#include "virtual_image_helpers.hpp"

#include <ranges>

#include <glm/vec3.hpp>

#include <core/image/Image.hpp>

#include "core/renderer/base/resources/image_extensions.hpp"

auto core::gfx::resources::sparse_color_requirements(
    const core::renderer::base::Image& image
) -> vk::SparseImageMemoryRequirements
{
    const std::vector<vk::SparseImageMemoryRequirements> sparse_image_memory_requirements{
        core::renderer::base::ext_sparse_memory_requirements(image)
    };

    const auto result{ std::ranges::find_if(
        sparse_image_memory_requirements,
        [](const vk::SparseImageMemoryRequirements& requirements) {
            return static_cast<bool>(
                requirements.formatProperties.aspectMask & vk::ImageAspectFlagBits::eColor
            );
        }
    ) };

    assert(
        result != std::ranges::end(sparse_image_memory_requirements)
        && "Sparse images without color aspect are not supported"
    );

    return *result;
}

[[nodiscard]]
static auto aligned_division(const vk::Extent3D& extent, const vk::Extent3D& granularity)
    -> glm::u32vec3
{
    return glm::u32vec3{
        extent.width / granularity.width
            + (((extent.width % granularity.width) != 0u) ? 1u : 0u),
        extent.height / granularity.height
            + (((extent.height % granularity.height) != 0u) ? 1u : 0u),
        extent.depth / granularity.depth
            + (((extent.depth % granularity.depth) != 0u) ? 1u : 0u),
    };
}

auto core::gfx::resources::create_sparse_blocks(
    const vk::Extent3D&                      extent,
    const uint32_t                           mip_level_count,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<VirtualImage::Block>
{
    assert(mip_level_count > sparse_requirements.imageMipTailFirstLod);

    std::vector<core::gfx::resources::VirtualImage::Block> result;

    const vk::Extent3D& image_granularity{
        sparse_requirements.formatProperties.imageGranularity
    };

    for (const uint32_t mip_level_index :
         std::views::iota(0u, sparse_requirements.imageMipTailFirstLod))
    {
        const vk::Extent3D mip_extent{
            .width  = std::max(extent.width >> mip_level_index, 1u),
            .height = std::max(extent.height >> mip_level_index, 1u),
            .depth  = std::max(extent.depth >> mip_level_index, 1u),
        };

        const glm::u32vec3 sparse_bind_counts{
            aligned_division(mip_extent, image_granularity)
        };

        const glm::u32vec3 last_block_extent{
            ((mip_extent.width % image_granularity.width) != 0u)
                ? mip_extent.width % image_granularity.width
                : image_granularity.width,
            ((mip_extent.height % image_granularity.height) != 0u)
                ? mip_extent.height % image_granularity.height
                : image_granularity.height,
            ((mip_extent.depth % image_granularity.depth) != 0u)
                ? mip_extent.depth % image_granularity.depth
                : image_granularity.depth,
        };

        const vk::ImageSubresource subresource{
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel   = mip_level_index,
            .arrayLayer = 0,
        };

        // TODO: use std::views::cartesian_product
        for (const uint32_t z : std::views::iota(0u, sparse_bind_counts.z)) {
            for (const uint32_t y : std::views::iota(0u, sparse_bind_counts.y)) {
                for (const uint32_t x : std::views::iota(0u, sparse_bind_counts.x)) {
                    const vk::Offset3D offset{
                        .x = static_cast<int32_t>(x * image_granularity.width),
                        .y = static_cast<int32_t>(y * image_granularity.height),
                        .z = static_cast<int32_t>(z * image_granularity.depth),
                    };

                    const vk::Extent3D block_extent{
                        .width  = (x == sparse_bind_counts.x - 1)
                                    ? last_block_extent.x
                                    : image_granularity.width,
                        .height = (y == sparse_bind_counts.y - 1)
                                    ? last_block_extent.y
                                    : image_granularity.height,
                        .depth  = (z == sparse_bind_counts.z - 1)
                                    ? last_block_extent.z
                                    : image_granularity.depth,
                    };

                    result.push_back(core::gfx::resources::VirtualImage::Block{
                        .m_offset      = offset,
                        .m_extent      = block_extent,
                        .m_size        = memory_requirements.alignment,
                        .m_subresource = subresource,
                    });
                }
            }
        }
    }

    return result;
}

auto core::gfx::resources::create_mip_tail_region(
    const core::renderer::base::Allocator&   allocator,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> VirtualImage::MipTailRegion
{
    const vk::MemoryRequirements mip_tail_memory_requirements{
        .size           = sparse_requirements.imageMipTailSize,
        .alignment      = memory_requirements.alignment,
        .memoryTypeBits = memory_requirements.memoryTypeBits,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    return VirtualImage::MipTailRegion{
        .m_memory = std::get<core::renderer::base::Allocation>(
            allocator.allocate(mip_tail_memory_requirements, allocation_create_info)
        ),
    };
}

auto core::gfx::resources::stage_tail(
    const core::renderer::base::Allocator&   allocator,
    const core::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements,
    const VirtualImage::MipTailRegion&       tail
) -> core::renderer::resources::SeqWriteBuffer<>
{
    const vk::BufferCreateInfo create_info{
        .size  = tail.m_memory.memory_view().size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    core::renderer::resources::SeqWriteBuffer<> result{ allocator, create_info };

    result.set(source.data().subspan(
        source.offset_of(sparse_requirements.imageMipTailFirstLod, 0, 0)
    ));

    return result;
}

auto core::gfx::resources::create_mip_tail_copy_regions(
    const core::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<vk::BufferImageCopy>
{
    std::vector<vk::BufferImageCopy> regions;
    regions.reserve(source.mip_level_count() - sparse_requirements.imageMipTailFirstLod);

    const vk::DeviceSize base_offset{
        source.offset_of(sparse_requirements.imageMipTailFirstLod, 0, 0)
    };

    for (const uint32_t i : std::views::iota(
             sparse_requirements.imageMipTailFirstLod, source.mip_level_count()
         ))
    {
        const vk::DeviceSize buffer_offset{ source.offset_of(i, 0, 0) - base_offset };

        const vk::Extent3D image_extent{
            .width  = std::max(source.width() >> i, 1u),
            .height = std::max(source.height() >> i, 1u),
            .depth  = 1,
        };

        const vk::BufferImageCopy region{
            .bufferOffset = buffer_offset,
            .imageSubresource =
                vk::ImageSubresourceLayers{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                           .mipLevel   = i,
                                           .baseArrayLayer = 0,
                                           .layerCount     = 1 },
            .imageExtent = image_extent,
        };

        regions.push_back(region);
    }

    return regions;
}
