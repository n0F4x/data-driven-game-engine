module;

#include <format>
#include <numeric>
#include <ranges>

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_format_traits.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <vk_mem_alloc.h>

#include <VkBootstrap.h>

#include <glm/vec3.hpp>

#include "modules/log/log_macros.hpp"

module modules.gfx.resources.VirtualImage;

import modules.image.Image;

import modules.gfx.resources.image_helpers;
import modules.gfx.resources.virtual_image_helpers;

import modules.log;

import modules.renderer.base.resources.Allocation;
import modules.renderer.base.resources.image_extensions;
import modules.renderer.base.resources.MemoryView;
import modules.renderer.resources.SeqWriteBuffer;

[[nodiscard]]
static auto image_usage_flags() -> vk::ImageUsageFlags
{
    return vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
}

[[nodiscard]]
static auto create_image(
    const vk::PhysicalDevice  physical_device,
    const vk::Device          device,
    const modules::image::Image& source,
    const vk::ImageUsageFlags usage
) -> modules::renderer::base::Image
{
    if (!vkuFormatIsColor(static_cast<VkFormat>(source.format()))) {
        ENGINE_LOG_ERROR(
            std::format(
                "Sparse image creation is only supported for color formats (given " "form" "at " "was " "`{}`)",
                vk::to_string(source.format())
            )
        );
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
        ENGINE_LOG_ERROR(
            std::format(
                "Sparse image creation are not supported for given format `{}`",
                vk::to_string(source.format())
            )
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

    return modules::renderer::base::Image{ device.createImageUnique(image_create_info),
                                        image_create_info };
}

[[nodiscard]]
static auto create_image_view(const modules::renderer::base::Image& image)
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

auto modules::gfx::resources::VirtualImage::Block::buffer_size(const vk::Format format) const
    -> uint64_t
{
    return static_cast<uint64_t>(vk::blockSize(format)) * m_extent.width * m_extent.height
         * m_extent.depth;
}

modules::gfx::resources::VirtualImage::Loader::Loader(
    const vk::PhysicalDevice         physical_device,
    const vk::Device                 device,
    const renderer::base::Allocator& allocator,
    const image::Image&              source
)
    : m_image{ ::create_image(physical_device, device, source, ::image_usage_flags()) },
      m_view{ ::create_image_view(m_image) },
      m_memory_requirements{ renderer::base::ext_memory_requirements(m_image) },
      m_sparse_requirements{ sparse_color_requirements(m_image) },
      m_blocks{ create_sparse_blocks(
          source,
          m_image.extent(),
          m_image.mip_level_count(),
          m_memory_requirements,
          m_sparse_requirements
      ) },
      m_mip_tail_region{
          create_mip_tail_region(allocator, m_memory_requirements, m_sparse_requirements)
      },
      m_mip_tail_staging_buffer{ stage_tail(allocator, source, m_sparse_requirements) },
      m_mip_tail_copy_regions{
          create_mip_tail_copy_regions(source, m_sparse_requirements)
      },
      m_allocator{ allocator },
      m_debug_image_loader{ device, allocator, source }
{
    if (const vk::DeviceSize sparse_address_space_size{
            physical_device.getProperties().limits.sparseAddressSpaceSize };
        m_memory_requirements.size > sparse_address_space_size)
    {
        ENGINE_LOG_ERROR(
            std::format(
                // clang-format off
                "Requested sparse image size ({}) exceeds supported sparse address space size ({})",
                // clang-format on
                m_memory_requirements.size,
                sparse_address_space_size
            )
        );
        assert(
            false
            && "Requested sparse image size exceeds supported sparse address space size"
        );
    }

    if (source.mip_level_count() <= m_sparse_requirements.imageMipTailFirstLod) {
        ENGINE_LOG_ERROR("Image source has not enough mip levels for it to be virtual");
        assert(false && "Image source has not enough mip levels for it to be virtual");
    }
}

auto modules::gfx::resources::VirtualImage::Loader::bind_tail(
    const vk::Queue sparse_queue
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

auto modules::gfx::resources::VirtualImage::Loader::operator()(
    const vk::PhysicalDevice            physical_device,
    const vk::Queue                     sparse_queue,
    const vk::CommandBuffer             transfer_command_buffer,
    const renderer::base::Image::State& new_state
) && -> VirtualImage
{
    bind_tail(sparse_queue);
    sparse_queue.waitIdle();

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
        m_mip_tail_copy_regions
    );

    transition_image_layout(transfer_command_buffer, m_image, new_state);

    VirtualImage result{ std::move(m_image),
                         std::move(m_view),
                         m_memory_requirements,
                         m_sparse_requirements,
                         std::move(m_blocks),
                         std::move(m_mip_tail_region),
                         std::move(m_debug_image_loader)(
                             physical_device, transfer_command_buffer, new_state
                         ) };

    return result;
}

auto modules::gfx::resources::VirtualImage::Loader::view() const -> vk::ImageView
{
    return m_view.get();
}

auto modules::gfx::resources::VirtualImage::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& physical_device_selector
) -> void
{
    constexpr static vk::PhysicalDeviceFeatures features{
        .shaderResourceResidency = vk::True,
        .shaderResourceMinLod    = vk::True,
        .sparseBinding           = vk::True,
        .sparseResidencyImage2D  = vk::True,
    };
    physical_device_selector.set_required_features(features);
}

[[nodiscard]]
static auto allocate_block(
    const modules::renderer::base::Allocator&           allocator,
    const vk::MemoryRequirements&                    memory_requirements,
    const modules::gfx::resources::VirtualImage::Block& info
) -> modules::renderer::base::Allocation
{
    const vk::MemoryRequirements mip_tail_memory_requirements{
        .size           = info.m_size,
        .alignment      = memory_requirements.alignment,
        .memoryTypeBits = memory_requirements.memoryTypeBits,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    return std::get<modules::renderer::base::Allocation>(
        allocator.allocate(mip_tail_memory_requirements, allocation_create_info)
    );
}

auto modules::gfx::resources::VirtualImage::update(
    const renderer::base::Allocator& allocator,
    const vk::Queue                  sparse_queue,
    const vk::CommandBuffer          transfer_command_buffer
) -> void
{
    for (const uint32_t index : std::views::iota(0u, m_blocks.size())) {
        if (m_to_be_loaded_mask.at(index)) {
            if (Block & block{ m_blocks.at(index) }; !block.m_allocation.has_value()) {
                block.m_allocation =
                    ::allocate_block(allocator, m_memory_requirements, block);
                assert(block.m_bound == false);
                assert(block.m_uploaded == false);
            }
        }
    }

    bind_memory_blocks(sparse_queue);
    upload_new_memory_blocks(allocator, transfer_command_buffer);

    for (const uint32_t index : std::views::iota(0u, m_blocks.size())) {
        if (m_to_be_unloaded_mask.at(index) && !m_to_be_loaded_mask.at(index)) {
            Block& block{ m_blocks.at(index) };
            block.m_uploaded = false;
            block.m_bound    = false;
            block.m_allocation.reset();
        }

        m_to_be_loaded_mask.at(index) = false;
    }
}

auto modules::gfx::resources::VirtualImage::clean_up_after_update() -> void
{
    m_staging_buffer.reset();
}

auto modules::gfx::resources::VirtualImage::image() const -> const renderer::base::Image&
{
    return m_image;
}

auto modules::gfx::resources::VirtualImage::view() const -> vk::ImageView
{
    return m_view.get();
}

auto modules::gfx::resources::VirtualImage::debug_image() const -> const Image&
{
    return m_debug_image;
}

auto modules::gfx::resources::VirtualImage::sparse_properties() const
    -> const vk::SparseImageMemoryRequirements&
{
    return m_sparse_requirements;
}

auto modules::gfx::resources::VirtualImage::blocks() const -> std::span<const Block>
{
    return m_blocks;
}

auto modules::gfx::resources::VirtualImage::request_block(const uint32_t block_index) -> void
{
    // ENGINE_LOG_DEBUG("Requested block at index {}", block_index);
    m_to_be_loaded_mask.at(block_index) = true;
}

auto modules::gfx::resources::VirtualImage::request_blocks_by_distance_from_camera(
    const double distance,
    const double map_scale
) -> void
{
    const double magic_distance{ 0.3 * map_scale };
    const double delta{ std::max(distance / magic_distance, 1.0) };
    const double lod{ std::log2(delta) };

    uint32_t request_block_count{};

    std::ranges::for_each(
        // TODO: std::views::enumerate
        std::views::zip(m_blocks, std::views::iota(0z))
            | std::views::drop_while([lod](auto&& block_and_index) {
                  return std::get<0>(block_and_index).m_subresource.mipLevel < lod;
              })
            | std::views::elements<1>,
        [this, &request_block_count](const std::size_t block_index) {
            m_to_be_loaded_mask.at(block_index) = true;
            request_block_count++;
        }
    );

    // if (request_block_count != 0) {
    //     ENGINE_LOG_DEBUG("Request {} blocks by distance", request_block_count);
    // }
}

auto modules::gfx::resources::VirtualImage::request_all_blocks() -> void
{
    std::ranges::for_each(
        std::views::iota(0u, m_to_be_loaded_mask.size()),
        [this](const std::size_t index) { m_to_be_loaded_mask.at(index) = true; }
    );
}

modules::gfx::resources::VirtualImage::VirtualImage(
    renderer::base::Image&&                  image,
    vk::UniqueImageView&&                    view,
    const vk::MemoryRequirements&            memory_requirements,
    const vk::SparseImageMemoryRequirements& sparse_requirements,
    std::vector<Block>&&                     blocks,
    MipTailRegion&&                          mip_tail_region,
    Image&&                                  debug_image
)
    : m_image{ std::move(image) },
      m_view{ std::move(view) },
      m_memory_requirements{ memory_requirements },
      m_sparse_requirements{ sparse_requirements },
      m_blocks{ std::move(blocks) },
      m_mip_tail_region{ std::move(mip_tail_region) },
      m_to_be_loaded_mask(m_blocks.size()),
      m_to_be_unloaded_mask(m_blocks.size(), true),
      m_debug_image{ std::move(debug_image) }
{}

auto modules::gfx::resources::VirtualImage::bind_memory_blocks(const vk::Queue sparse_queue)
    -> void
{
    // TODO: use enumerate
    const std::vector<vk::SparseImageMemoryBind> image_memory_binds{
        std::views::zip(
            m_to_be_loaded_mask,
            m_to_be_unloaded_mask,
            std::views::iota(0u, m_blocks.size())
        )
        | std::views::filter([&](std::tuple<bool, bool, std::size_t> zipped) {
              const bool        to_be_loaded{ std::get<0>(zipped) };
              const bool        to_be_unloaded{ std::get<1>(zipped) };
              const std::size_t block_index{ std::get<2>(zipped) };
              const Block&      block{ m_blocks.at(block_index) };

              return (to_be_loaded && !block.m_bound)
                  || (!to_be_loaded && to_be_unloaded && block.m_bound);
          })
        | std::views::transform([&](std::tuple<bool, bool, std::size_t> zipped) {
              const bool        to_be_loaded{ std::get<0>(zipped) };
              const std::size_t block_index{ std::get<2>(zipped) };
              Block&            block{ m_blocks.at(block_index) };

              block.m_bound = !block.m_bound;

              return to_be_loaded ?
                  vk::SparseImageMemoryBind{
                      .subresource  = block.m_subresource,
                      .offset       = block.m_offset,
                      .extent       = block.m_extent,
                      .memory       = block.m_allocation->memory_view().memory,
                      .memoryOffset = block.m_allocation->memory_view().offset,
                  }
                : vk::SparseImageMemoryBind{
                      .subresource = block.m_subresource,
                      .offset      = block.m_offset,
                      .extent      = block.m_extent,
                      .memory      = nullptr,
                };
          })
        | std::ranges::to<std::vector>()
    };

    if (!image_memory_binds.empty()) {
        const vk::SparseImageMemoryBindInfo image_memory_bind_info{
            .image     = m_image.get(),
            .bindCount = static_cast<uint32_t>(image_memory_binds.size()),
            .pBinds    = image_memory_binds.data(),
        };
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
            .imageBindCount       = 1,
            .pImageBinds          = &image_memory_bind_info,
        };
        sparse_queue.bindSparse(bind_sparse_info);
    }
}

[[nodiscard]]
static auto create_copy_regions(
    const vk::Format                format,
    std::ranges::forward_range auto memory_blocks
) -> std::vector<vk::BufferImageCopy>
{
    return memory_blocks
         | std::views::transform(
               [offset = 0u, format](
                   const modules::gfx::resources::VirtualImage::Block& block
               ) mutable -> vk::BufferImageCopy {
                   const vk::BufferImageCopy result{
                       .bufferOffset = offset,
                       .imageSubresource =
                           vk::ImageSubresourceLayers{
                                                      .aspectMask     = block.m_subresource.aspectMask,
                                                      .mipLevel       = block.m_subresource.mipLevel,
                                                      .baseArrayLayer = block.m_subresource.arrayLayer,
                                                      .layerCount     = 1 },
                       .imageOffset = block.m_offset,
                       .imageExtent = block.m_extent,
                   };

                   offset += block.buffer_size(format);

                   return result;
               }
         )
         | std::ranges::to<std::vector>();
}

[[nodiscard]]
static auto create_staging_buffer(
    const modules::renderer::base::Allocator& allocator,
    const vk::Format                       format,
    std::ranges::forward_range auto        blocks
) -> modules::renderer::resources::SeqWriteBuffer<>
{
    const vk::BufferCreateInfo staging_buffer_create_info{
        // TODO: use std::ranges::fold_left_first
        .size = std::ranges::fold_left(
            blocks
                | std::views::transform(
                    std::bind_back(
                        &modules::gfx::resources::VirtualImage::Block::buffer_size, format
                    )
                ),
            0u,
            std::plus<>{}
        ),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    modules::renderer::resources::SeqWriteBuffer<> result{ allocator,
                                                        staging_buffer_create_info };

    std::ranges::for_each(
        blocks,
        [&result,
         format,
         offset = 0u](const modules::gfx::resources::VirtualImage::Block& block) mutable {
            result.set(std::span{ block.m_source }, offset);
            offset += block.buffer_size(format);
        }
    );

    return result;
}

auto modules::gfx::resources::VirtualImage::upload_new_memory_blocks(
    const modules::renderer::base::Allocator& allocator,
    const vk::CommandBuffer                transfer_command_buffer
) -> void
{
    std::ranges::view auto blocks_to_be_uploaded{
        m_blocks | std::views::filter([](const Block& block) {
            return block.m_bound && !block.m_uploaded;
        })
    };

    if (!std::ranges::empty(blocks_to_be_uploaded)) {
        const std::vector<vk::BufferImageCopy> copy_regions{ ::create_copy_regions(
            m_image.format(), std::ranges::ref_view{ blocks_to_be_uploaded }
        ) };

        m_staging_buffer = ::create_staging_buffer(
            allocator, m_image.format(), std::ranges::ref_view{ blocks_to_be_uploaded }
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

    std::ranges::for_each(blocks_to_be_uploaded, [](Block& block) {
        block.m_uploaded = true;
    });
}
