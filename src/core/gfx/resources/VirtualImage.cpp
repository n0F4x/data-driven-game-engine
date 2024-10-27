#include "VirtualImage.hpp"

#include <ranges>

#include <spdlog/spdlog.h>

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_to_string.hpp>

#include <glm/vec3.hpp>

#include <core/renderer/base/resources/image_extensions.hpp>
#include <core/renderer/execution/Executor.hpp>
#include <core/renderer/resources/RandomAccessBuffer.hpp>

#include "core/image/Image.hpp"
#include "core/renderer/base/resources/Allocation.hpp"

#include "image_helpers.hpp"
#include "virtual_image_helpers.hpp"

[[nodiscard]]
static auto image_usage_flags(const bool needs_to_blit_mipmaps) -> vk::ImageUsageFlags
{
    vk::ImageUsageFlags result{ vk::ImageUsageFlagBits::eTransferDst
                                | vk::ImageUsageFlagBits::eSampled };

    if (needs_to_blit_mipmaps) {
        result |= vk::ImageUsageFlagBits::eTransferSrc;
    }

    return result;
}

[[nodiscard]]
static auto count_mip_levels(const uint32_t base_width, const uint32_t base_height)
    -> uint32_t
{
    return static_cast<uint32_t>(std::floor(std::log2(std::max(base_width, base_height))))
         + 1;
}

[[nodiscard]]
static auto create_image(
    const vk::PhysicalDevice  physical_device,
    const vk::Device          device,
    const vk::Format          format,
    const vk::Extent3D&       extent,
    const uint32_t            mip_level_count,
    const vk::ImageUsageFlags usage
) -> core::renderer::base::Image
{
    if (!vkuFormatIsColor(static_cast<VkFormat>(format))) {
        SPDLOG_ERROR(
            "Sparse image creation is only supported for color formats (given format was "
            "`{}`)",
            vk::to_string(format)
        );
        spdlog::shutdown();
        assert(false && "Sparse image creation is only supported for color formats");
    }

    if (physical_device
            .getSparseImageFormatProperties(
                format,
                vk::ImageType::e2D,
                vk::SampleCountFlagBits::e1,
                usage,
                vk::ImageTiling::eOptimal
            )
            .empty())
    {
        SPDLOG_ERROR(
            "Sparse image creation are not supported for given format `{}`",
            vk::to_string(format)
        );
        assert(false && "Sparse image creation are not supported for given format");
    }

    const vk::ImageCreateInfo image_create_info{
        .imageType     = vk::ImageType::e2D,
        .format        = format,
        .extent        = extent,
        .mipLevels     = mip_level_count,
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

core::gfx::resources::VirtualImage::Loader::Loader(
    const vk::PhysicalDevice         physical_device,
    const vk::Device                 device,
    const renderer::base::Allocator& allocator,
    const core::image::Image&        source
)
    : m_raw_image_loader{ device, allocator, source },
      m_image{ ::create_image(
          physical_device,
          device,
          source.format(),
          vk::Extent3D{ source.width(), source.height(), source.depth() },
          ::count_mip_levels(source.width(), source.height()),
          ::image_usage_flags(source.needs_mip_generation())
      ) },
      m_view{ ::create_image_view(m_image) },
      m_element_size{ vkuFormatElementSize(static_cast<VkFormat>(m_image.format())) },
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

// [[nodiscard]]
// static auto create_readback_buffer(
//     const core::renderer::base::Allocator& allocator,
//     const vk::DeviceSize                   buffer_size
// ) -> core::renderer::resources::RandomAccessBuffer<>
// {
//     const vk::BufferCreateInfo create_info{
//         .size  = buffer_size,
//         .usage = vk::BufferUsageFlagBits::eTransferDst,
//     };
//
//     return core::renderer::resources::RandomAccessBuffer<>{ allocator, create_info };
// }
//
// [[nodiscard]]
// static auto buffer_image_copy_regions(const core::gfx::resources::Image& image)
//     -> std::vector<vk::BufferImageCopy>
// {
//     return std::views::iota(0u, image.get().mip_level_count())
//          | std::views::transform([&image](const uint32_t mip_level_index) {
//                const vk::ImageSubresourceLayers image_subresource_layers{
//                    .aspectMask = vk::ImageAspectFlagBits::eColor,
//                    .mipLevel   = mip_level_index,
//                    .layerCount = 1,
//                };
//
//                return vk::BufferImageCopy{
//                    // .bufferRowLength = 0, ???
//                    // .bufferImageHeight = 0, ???
//                    .imageSubresource = image_subresource_layers,
//                    .imageExtent      = image.get().extent(),
//                };
//            })
//          | std::ranges::to<std::vector>();
// }

// static auto copy_image_to_buffer(
//     const vk::CommandBuffer                                transfer_command_buffer,
//     const core::gfx::resources::Image&                     image,
//     const core::renderer::resources::RandomAccessBuffer<>& buffer
// ) -> void
// {
//     transfer_command_buffer.copyImageToBuffer(
//         image.get().get(),
//         image.get().layout(),
//         buffer.buffer(),
//         ::buffer_image_copy_regions(image)
//     );
// }
//
// static auto read_back_raw_image(
//     const vk::PhysicalDevice               physical_device,
//     const core::renderer::base::Allocator& allocator,
//     const core::renderer::Executor&        executor,
//     core::gfx::resources::Image::Loader&&  raw_image_loader
// ) -> std::vector<core::renderer::resources::RandomAccessBuffer<>>
// {
//     return executor.execute_single_command(
//         [physical_device,
//          &raw_image_loader,
//          &allocator](const vk::CommandBuffer immediate_graphics_command_buffer) {
//             const core::gfx::resources::Image image{ std::move(raw_image_loader)(
//                 physical_device,
//                 immediate_graphics_command_buffer,
//                 core::renderer::base::Image::State{
//                     vk::PipelineStageFlagBits::eTransfer,
//                     vk::AccessFlagBits::eTransferRead,
//                     vk::ImageLayout::eTransferSrcOptimal,
//                 }
//             ) };
//
//             core::renderer::resources::RandomAccessBuffer<> buffer{
//                 ::create_readback_buffer(
//                     allocator, image.get().allocation().memory_view().size
//                 )
//             };
//
//             ::copy_image_to_buffer(immediate_graphics_command_buffer, image, buffer);
//
//             return buffer;
//         }
//     );
// }
//
// auto core::gfx::resources::VirtualImage::Loader::operator()(
//     const vk::PhysicalDevice  physical_device,
//     const vk::Queue           sparse_queue,
//     const vk::CommandBuffer   graphics_command_buffer,
//     const renderer::Executor& executor
// ) && -> VirtualImage
// {
//     const core::renderer::resources::RandomAccessBuffer<> readback_raw_image_buffer{
//         ::read_back_raw_image(
//             physical_device, m_allocator, executor, std::move(m_raw_image_loader)
//         )
//     };
//
//     std::vector<std::byte> raw_image_data{};
//     raw_image_data.resize(readback_raw_image_buffer.size());
//     readback_raw_image_buffer.get(std::span{ raw_image_data });
//
//     transition_image_layout(
//         graphics_command_buffer,
//         m_image,
//         renderer::base::Image::State{
//             vk::PipelineStageFlagBits::eAllCommands,
//             vk::AccessFlagBits::eShaderRead,
//             vk::ImageLayout::eShaderReadOnlyOptimal,
//         }
//     );
//
//     bind_tail(sparse_queue);
//
//     return VirtualImage{ std::move(raw_image_data),
//                          std::move(m_image),
//                          std::move(m_view) };
// }

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

core::gfx::resources::VirtualImage::VirtualImage(
    std::vector<std::byte>&& image_data,
    renderer::base::Image&&  image,
    vk::UniqueImageView&&    view
) noexcept
    : m_image_data{ std::move(image_data) },
      m_image{ std::move(image) },
      m_view{ std::move(view) }
{}
