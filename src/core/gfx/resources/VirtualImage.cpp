#include "VirtualImage.hpp"

#include <numeric>
#include <ranges>

#include <spdlog/spdlog.h>

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_format_traits.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <glm/vec3.hpp>

#include "core/image/Image.hpp"
#include "core/renderer/base/resources/Allocation.hpp"
#include "core/renderer/base/resources/image_extensions.hpp"

#include "image_helpers.hpp"
#include "virtual_image_helpers.hpp"

[[nodiscard]]
static auto image_usage_flags() -> vk::ImageUsageFlags
{
    return vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
}

[[nodiscard]]
static auto create_image(
    const vk::PhysicalDevice  physical_device,
    const vk::Device          device,
    const core::image::Image& source,
    const vk::ImageUsageFlags usage
) -> core::renderer::base::Image
{
    if (!vkuFormatIsColor(static_cast<VkFormat>(source.format()))) {
        SPDLOG_ERROR(
            "Sparse image creation is only supported for color formats (given format was "
            "`{}`)",
            vk::to_string(source.format())
        );
        spdlog::shutdown();
        assert(false && "Sparse image creation is only supported for color formats");
    }

    if (physical_device
            .getSparseImageFormatProperties(
                source.format(),
                vk::ImageType::e2D,
                vk::SampleCountFlagBits::e1,
                usage,
                vk::ImageTiling::eOptimal
            )
            .empty())
    {
        SPDLOG_ERROR(
            "Sparse image creation are not supported for given format `{}`",
            vk::to_string(source.format())
        );
        assert(false && "Sparse image creation are not supported for given format");
    }

    const vk::ImageCreateInfo image_create_info{
        .flags = vk::ImageCreateFlagBits::eSparseBinding
               | vk::ImageCreateFlagBits::eSparseResidency,
        .imageType     = vk::ImageType::e2D,
        .format        = source.format(),
        .extent        = source.extent(),
        .mipLevels     = source.mip_level_count(),
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = vk::ImageTiling::eOptimal,
        .usage         = usage,
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    return core::renderer::base::Image{ device.createImageUnique(image_create_info),
                                        image_create_info };
}

[[nodiscard]]
static auto create_image_view(const core::renderer::base::Image& image)
    -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo view_create_info{
        .image    = image.get(),
        .viewType = vk::ImageViewType::e2D,
        .format   = image.format(),
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                      .levelCount = image.mip_level_count(),
                                      .layerCount = 1 },
    };

    return image.device().createImageViewUnique(view_create_info);
}

auto core::gfx::resources::VirtualImage::Block::buffer_size(const vk::Format format) const
    -> uint64_t
{
    return static_cast<uint64_t>(vk::blockSize(format)) * m_extent.width * m_extent.height
         * m_extent.depth;
}

core::gfx::resources::VirtualImage::Loader::Loader(
    const vk::PhysicalDevice         physical_device,
    const vk::Device                 device,
    const renderer::base::Allocator& allocator,
    std::unique_ptr<image::Image>&&  source
)
    : m_source{ std::move(source) },
      m_image{ ::create_image(physical_device, device, *m_source, ::image_usage_flags()) },
      m_view{ ::create_image_view(m_image) },
      m_memory_requirements{ renderer::base::ext_memory_requirements(m_image) },
      m_sparse_requirements{ sparse_color_requirements(m_image) },
      m_blocks{ create_sparse_blocks(
          m_image.extent(),
          m_image.mip_level_count(),
          m_memory_requirements,
          m_sparse_requirements
      ) },
      m_mip_tail_region{
          create_mip_tail_region(allocator, m_memory_requirements, m_sparse_requirements)
      },
      m_mip_tail_staging_buffer{
          stage_tail(allocator, *m_source, m_sparse_requirements, m_mip_tail_region)
      },
      m_allocator{ allocator }
{
    // Check requested image size against hardware sparse limit
    if (const vk::DeviceSize sparse_address_space_size{
            physical_device.getProperties().limits.sparseAddressSpaceSize };
        m_memory_requirements.size > sparse_address_space_size)
    {
        SPDLOG_ERROR(
            "Requested sparse image size ({}) "
            "exceeds supported sparse address space size ({})",
            m_memory_requirements.size,
            sparse_address_space_size
        );
        spdlog::shutdown();
        assert(
            false
            && "Requested sparse image size exceeds supported sparse address space size"
        );
    }

    if (m_source->mip_level_count() <= m_sparse_requirements.imageMipTailFirstLod) {
        SPDLOG_ERROR("Image source has not enough mip levels for it to be virtual");
        spdlog::shutdown();
        assert(false && "Image source has not enough mip levels for it to be virtual");
    }
}

auto core::gfx::resources::VirtualImage::Loader::bind_tail(const vk::Queue sparse_queue
) const -> void
{
    const renderer::base::MemoryView mip_tail_memory_view{
        m_mip_tail_region.m_memory.memory_view()
    };
    const vk::SparseMemoryBind opaque_memory_bind{
        .resourceOffset = m_sparse_requirements.imageMipTailOffset,
        .size           = mip_tail_memory_view.size,
        .memory         = mip_tail_memory_view.memory,
        .memoryOffset   = mip_tail_memory_view.offset
    };
    const vk::SparseImageOpaqueMemoryBindInfo image_opaque_memory_bind_info{
        .image     = m_image.get(),
        .bindCount = 1,
        .pBinds    = &opaque_memory_bind,
    };
    const vk::BindSparseInfo bind_sparse_info{
        .imageOpaqueBindCount = 1,
        .pImageOpaqueBinds    = &image_opaque_memory_bind_info,
    };
    sparse_queue.bindSparse(bind_sparse_info);
}

auto core::gfx::resources::VirtualImage::Loader::operator()(
    const vk::Queue                     sparse_queue,
    const vk::CommandBuffer             transfer_command_buffer,
    const renderer::base::Image::State& new_state
) && -> VirtualImage
{
    bind_tail(sparse_queue);

    transition_image_layout(
        transfer_command_buffer,
        m_image,
        renderer::base::Image::State{
            vk::PipelineStageFlagBits::eTransfer,
            vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eTransferDstOptimal,
        }
    );

    transfer_command_buffer.copyBufferToImage(
        m_mip_tail_staging_buffer.get(),
        m_image.get(),
        m_image.layout(),
        create_mip_tail_copy_regions(*m_source, m_sparse_requirements)
    );

    transition_image_layout(transfer_command_buffer, m_image, new_state);

    VirtualImage result{ std::move(m_source),         std::move(m_image),
                         std::move(m_view),           m_memory_requirements,
                         m_sparse_requirements,       std::move(m_blocks),
                         std::move(m_mip_tail_region) };

    return result;
}

auto core::gfx::resources::VirtualImage::Loader::view() const -> vk::ImageView
{
    return m_view.get();
}

auto core::gfx::resources::VirtualImage::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& physical_device_selector
) -> void
{
    constexpr static vk::PhysicalDeviceFeatures features{
        .shaderResourceResidency = vk::True,
        .sparseBinding           = vk::True,
        .sparseResidencyImage2D  = vk::True,
    };
    physical_device_selector.set_required_features(features);
}

[[nodiscard]]
static auto allocate_block(
    const core::renderer::base::Allocator&           allocator,
    const vk::MemoryRequirements&                    memory_requirements,
    const core::gfx::resources::VirtualImage::Block& info
) -> core::renderer::base::Allocation
{
    const vk::MemoryRequirements mip_tail_memory_requirements{
        .size           = info.m_size,
        .alignment      = memory_requirements.alignment,
        .memoryTypeBits = memory_requirements.memoryTypeBits,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    return std::get<core::renderer::base::Allocation>(
        allocator.allocate(mip_tail_memory_requirements, allocation_create_info)
    );
}

auto core::gfx::resources::VirtualImage::update(
    const renderer::base::Allocator& allocator,
    const vk::Queue                  sparse_queue,
    const vk::CommandBuffer          transfer_command_buffer
) -> void
{
    for (const uint32_t index : m_to_be_unloaded_block_indices) {
        m_blocks[index].m_allocation.reset();
    }
    for (const uint32_t index : m_to_be_loaded_block_indices) {
        m_blocks[index].m_allocation =
            ::allocate_block(allocator, m_memory_requirements, m_blocks[index]);
    }

    bind_memory_blocks(sparse_queue);
    upload_new_memory_blocks(allocator, transfer_command_buffer);

    m_to_be_unloaded_block_indices.clear();
    m_to_be_loaded_block_indices.clear();

    // TODO: remove this artificial request
    m_to_be_unloaded_block_indices = std::views::iota(0u, m_blocks.size())
                                   | std::ranges::to<std::vector>();
    m_to_be_loaded_block_indices = std::views::iota(0u, m_blocks.size())
                                 | std::ranges::to<std::vector>();
}

auto core::gfx::resources::VirtualImage::clean_up_after_update() -> void
{
    m_staging_buffer.reset();
}

core::gfx::resources::VirtualImage::VirtualImage(
    std::unique_ptr<image::Image>&&          source,
    renderer::base::Image&&                  image,
    vk::UniqueImageView&&                    view,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements,
    std::vector<Block>&&                     blocks,
    MipTailRegion&&                          mip_tail_region
)
    : m_source{ std::move(source) },
      m_image{ std::move(image) },
      m_view{ std::move(view) },
      m_memory_requirements{ memory_requirements },
      m_sparse_requirements{ sparse_requirements },
      m_blocks{ std::move(blocks) },
      m_mip_tail_region{ std::move(mip_tail_region) }
{}

auto core::gfx::resources::VirtualImage::bind_memory_blocks(const vk::Queue sparse_queue)
    -> void
{
    const std::vector<vk::SparseImageMemoryBind> image_memory_binds{
        std::array{ m_to_be_loaded_block_indices, m_to_be_unloaded_block_indices }
        | std::views::join | std::views::transform([&](const uint32_t index) {
              return std::ref(m_blocks[index]);
                   }
        )
        | std::views::transform([&](const Block& block) {
              if (block.m_allocation.has_value()) {
                  return vk::SparseImageMemoryBind{
                      .subresource  = block.m_subresource,
                      .offset       = block.m_offset,
                      .extent       = block.m_extent,
                      .memory       = block.m_allocation->memory_view().memory,
                      .memoryOffset = block.m_allocation->memory_view().offset,
                  };
              }
              return vk::SparseImageMemoryBind{
                  .subresource = block.m_subresource,
                  .offset      = block.m_offset,
                  .extent      = block.m_extent,
                  .memory      = nullptr,
              };
                   }
        )
        | std::ranges::to<std::vector>()
    };

    if (!image_memory_binds.empty()) {
        const vk::SparseImageMemoryBindInfo image_memory_bind_info{
            .image     = m_image.get(),
            .bindCount = static_cast<uint32_t>(image_memory_binds.size()),
            .pBinds    = image_memory_binds.data(),
        };
        const vk::BindSparseInfo bind_sparse_info{
            .imageBindCount = 1,
            .pImageBinds    = &image_memory_bind_info,
        };
        sparse_queue.bindSparse(bind_sparse_info);
    }
}

[[nodiscard]]
static auto create_copy_regions(std::ranges::sized_range auto memory_blocks)
    -> std::vector<vk::BufferImageCopy>
{
    return memory_blocks
         | std::views::transform(
               [offset = 0u](const core::gfx::resources::VirtualImage::Block& block
               ) mutable -> vk::BufferImageCopy {
                   const vk::BufferImageCopy result{
                       .bufferOffset = offset,
                       .imageSubresource =
                           vk::ImageSubresourceLayers{
                                                      .aspectMask     = block.m_subresource.aspectMask,
                                                      .mipLevel       = block.m_subresource.mipLevel,
                                                      .baseArrayLayer = block.m_subresource.arrayLayer,
                                                      .layerCount     = 1 },
                       .imageExtent = block.m_extent,
                   };

                   offset += block.m_size;

                   return result;
               }
         )
         | std::ranges::to<std::vector>();
}

[[nodiscard]]
static auto create_staging_buffer(
    const core::renderer::base::Allocator& allocator,
    const core::image::Image&              source,
    std::ranges::range auto                blocks
) -> core::renderer::resources::SeqWriteBuffer<>
{
    const vk::BufferCreateInfo staging_buffer_create_info{
        // TODO: use std::ranges::fold_left_first
        .size = std::ranges::fold_left(
            blocks
                | std::views::transform(
                    // TODO: use bind_back
                    [&source](const core::gfx::resources::VirtualImage::Block& block) {
                        return block.buffer_size(source.format());
                    }
                ),
            0u,
            std::plus<>{}
        ),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    core::renderer::resources::SeqWriteBuffer<> result{ allocator,
                                                        staging_buffer_create_info };

    std::ranges::for_each(
        blocks,
        [&result,
         &source,
         offset = 0u](const core::gfx::resources::VirtualImage::Block& block) mutable {
            const auto data{ source.data().subspan(
                source.offset_of(
                    block.m_subresource.mipLevel, block.m_subresource.arrayLayer, 0
                ),
                block.buffer_size(source.format())
            ) };
            result.set(data, offset);
            offset += data.size_bytes();
        }
    );

    return result;
}

auto core::gfx::resources::VirtualImage::upload_new_memory_blocks(
    const core::renderer::base::Allocator& allocator,
    const vk::CommandBuffer                transfer_command_buffer
) -> void
{
    if (!m_to_be_loaded_block_indices.empty()) {
        auto blocks_to_be_uploaded{ m_to_be_loaded_block_indices
                                    | std::views::transform([&](const uint32_t index) {
                                          return std::ref(m_blocks[index]);
                                      }) };

        const std::vector<vk::BufferImageCopy> copy_regions{
            ::create_copy_regions(std::ranges::ref_view{ blocks_to_be_uploaded })
        };

        m_staging_buffer = ::create_staging_buffer(
            allocator, *m_source, std::ranges::ref_view{ blocks_to_be_uploaded }
        );

        const auto old_state{ transition_image_layout(
            transfer_command_buffer,
            m_image,
            renderer::base::Image::State{
                .stage_mask  = vk::PipelineStageFlagBits::eTransfer,
                .access_mask = vk::AccessFlagBits::eTransferWrite,
                .layout      = vk::ImageLayout::eTransferDstOptimal,
            }
        ) };

        transfer_command_buffer.copyBufferToImage(
            m_staging_buffer->get(), m_image.get(), m_image.layout(), copy_regions
        );

        transition_image_layout(transfer_command_buffer, m_image, old_state);
    }
}