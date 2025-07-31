module;

#include <cstddef>
#include <ranges>

#include <vulkan/vulkan_format_traits.hpp>

#include <vk_mem_alloc.h>

#include <glm/vec3.hpp>

module core.gfx.resources.virtual_image_helpers;

import core.image.Image;

import core.renderer.base.allocator.Allocator;
import core.renderer.base.resources.Allocation;
import core.renderer.base.resources.image_extensions;
import core.renderer.resources.SeqWriteBuffer;

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

[[nodiscard]]
static auto aligned_modulo(const vk::Extent3D& extent, const vk::Extent3D& granularity)
    -> glm::u32vec3
{
    return glm::u32vec3{
        ((extent.width % granularity.width) != 0u) ? extent.width % granularity.width
                                                   : granularity.width,
        ((extent.height % granularity.height) != 0u) ? extent.height % granularity.height
                                                     : granularity.height,
        ((extent.depth % granularity.depth) != 0u) ? extent.depth % granularity.depth
                                                   : granularity.depth,
    };
}

[[nodiscard]]
static auto block_extent(
    const vk::Extent3D& image_granularity,
    const vk::Extent3D& mip_extent,
    const glm::u32vec3& sparse_bind_counts,
    const uint32_t      bind_index_x,
    const uint32_t      bind_index_y,
    const uint32_t      bind_index_z
) -> vk::Extent3D
{
    const glm::u32vec3 last_block_extent{
        ::aligned_modulo(mip_extent, image_granularity)
    };

    return vk::Extent3D{
        .width  = (bind_index_x == sparse_bind_counts.x - 1) ? last_block_extent.x
                                                             : image_granularity.width,
        .height = (bind_index_y == sparse_bind_counts.y - 1) ? last_block_extent.y
                                                             : image_granularity.height,
        .depth  = (bind_index_z == sparse_bind_counts.z - 1) ? last_block_extent.z
                                                             : image_granularity.depth,
    };
}

[[nodiscard]]
static auto create_block_source(
    const core::image::Image& source,
    const vk::Offset3D&       offset,
    const vk::Extent3D&       extent,
    const uint32_t            mip_level_index
) -> std::vector<std::byte>
{
    const std::size_t texel_block_size{ vk::blockSize(source.format()) };

    std::vector<std::byte> result;
    result.reserve(texel_block_size * extent.width * extent.height * extent.depth);

    const vk::Extent3D mip_extent{
        .width  = std::max(source.extent().width >> mip_level_index, 1u),
        .height = std::max(source.extent().height >> mip_level_index, 1u),
        .depth  = std::max(source.extent().depth >> mip_level_index, 1u),
    };

    for (const std::size_t z :
         std::views::iota(static_cast<std::size_t>(offset.z)) | std::views::take(extent.depth))
    {
        for (const std::size_t y : std::views::iota(static_cast<std::size_t>(offset.y))
                                  | std::views::take(extent.height))
        {
            for (const std::size_t x : std::views::iota(static_cast<std::size_t>(offset.x))
                                      | std::views::take(extent.width))
            {
                const std::size_t mip_offset{ z * mip_extent.height * mip_extent.width
                                         + y * mip_extent.width + x };

                result.append_range(source.data().subspan(
                    source.offset_of(mip_level_index, 0, 0)
                        + mip_offset * texel_block_size,
                    texel_block_size
                ));
            }
        }
    }

    return result;
}

auto core::gfx::resources::create_sparse_blocks(
    const image::Image&                      source,
    const vk::Extent3D&                      extent,
    [[maybe_unused]] const uint32_t          mip_level_count,
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

        const vk::ImageSubresource subresource{
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel   = mip_level_index,
            .arrayLayer = 0,
        };

        const glm::u32vec3 sparse_bind_counts{
            ::aligned_division(mip_extent, image_granularity)
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

                    const vk::Extent3D block_extent{ ::block_extent(
                        image_granularity, mip_extent, sparse_bind_counts, x, y, z
                    ) };

                    result.push_back(
                        core::gfx::resources::VirtualImage::Block{
                            .m_source = ::create_block_source(
                                source, offset, block_extent, mip_level_index
                            ),
                            .m_offset      = offset,
                            .m_extent      = block_extent,
                            .m_size        = memory_requirements.alignment,
                            .m_subresource = subresource,
                        }
                    );
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
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> core::renderer::resources::SeqWriteBuffer<>
{
    const std::span<const std::byte> data{ source.data().subspan(
        source.offset_of(sparse_requirements.imageMipTailFirstLod, 0, 0)
    ) };

    const vk::BufferCreateInfo create_info{
        .size  = data.size_bytes(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    core::renderer::resources::SeqWriteBuffer<> result{ allocator, create_info };

    result.set(data);

    return result;
}

auto core::gfx::resources::create_mip_tail_copy_regions(
    const core::image::Image&                source,
    const vk::SparseImageMemoryRequirements& sparse_requirements
) -> std::vector<vk::BufferImageCopy>
{
    std::vector<vk::BufferImageCopy> regions;
    regions.reserve(source.mip_level_count() - sparse_requirements.imageMipTailFirstLod);

    vk::DeviceSize buffer_offset{};

    for (const uint32_t i : std::views::iota(
             sparse_requirements.imageMipTailFirstLod, source.mip_level_count()
         ))
    {
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

        const vk::DeviceSize mip_size{
            static_cast<const vk::DeviceSize>(vk::blockSize(source.format()))
            * image_extent.width * image_extent.height * image_extent.depth
        };

        buffer_offset += mip_size;
    }

    return regions;
}
