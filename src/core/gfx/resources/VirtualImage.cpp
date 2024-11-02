#include "VirtualImage.hpp"

#include <ranges>

#include <spdlog/spdlog.h>

#include <vulkan/utility/vk_format_utils.h>
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

core::gfx::resources::VirtualImage::Loader::Loader(
    const vk::PhysicalDevice         physical_device,
    const vk::Device                 device,
    const renderer::base::Allocator& allocator,
    std::unique_ptr<image::Image>&&  source
)
    : m_source{ std::move(source) },
      m_image{ ::create_image(physical_device, device, *m_source, ::image_usage_flags()) },
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
      }
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
    const vk::Queue         sparse_queue,
    const vk::CommandBuffer graphics_command_buffer
) && -> VirtualImage
{
    transition_image_layout(
        graphics_command_buffer,
        m_image,
        renderer::base::Image::State{
            vk::PipelineStageFlagBits::eAllCommands,
            vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eShaderReadOnlyOptimal,
        }
    );

    bind_tail(sparse_queue);

    return VirtualImage{ std::move(m_source), std::move(m_image), std::move(m_view) };
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

core::gfx::resources::VirtualImage::VirtualImage(
    std::unique_ptr<image::Image>&& source,
    renderer::base::Image&&         image,
    vk::UniqueImageView&&           view
) noexcept
    : m_source{ std::move(source) },
      m_image{ std::move(image) },
      m_view{ std::move(view) }
{}
