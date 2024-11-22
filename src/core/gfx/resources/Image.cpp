module;

#include "core/renderer/base/resources/Image.hpp"

#include <cmath>
#include <ranges>

#include <VkBootstrap.h>

#include "core/renderer/base/resources/mipmap_generation.hpp"
#include "core/renderer/resources/Image.hpp"
#include "core/renderer/resources/SeqWriteBuffer.hpp"

module core.gfx.resources.Image;

import core.image.Image;

import core.gfx.resources.image_helpers;

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
    const core::renderer::base::Allocator& allocator,
    const core::image::Image&              source
) -> core::renderer::resources::Image
{
    const uint32_t mip_level_count{ source.needs_mip_generation()
                                        ? count_mip_levels(source.width(), source.height())
                                        : source.mip_level_count() };

    const vk::ImageCreateInfo image_create_info{
        .imageType     = vk::ImageType::e2D,
        .format        = source.format(),
        .extent        = vk::Extent3D{ source.width(), source.height(), source.depth() },
        .mipLevels     = mip_level_count,
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = vk::ImageTiling::eOptimal,
        .usage         = ::image_usage_flags(source.needs_mip_generation()),
        .sharingMode   = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info{
        .usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
    };

    return core::renderer::resources::Image{ allocator,
                                             image_create_info,
                                             allocation_create_info };
}

[[nodiscard]]
static auto create_image_view(
    const vk::Device device,
    const vk::Image  image,
    const vk::Format format,
    const uint32_t   mip_level_count
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo view_create_info{
        .image    = image,
        .viewType = vk::ImageViewType::e2D,
        .format   = format,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                      .levelCount = mip_level_count,
                                      .layerCount = 1 },
    };

    return device.createImageViewUnique(view_create_info);
}

[[nodiscard]]
static auto create_copy_regions(const core::image::Image& source)
    -> std::vector<vk::BufferImageCopy>
{
    const uint32_t mip_level_count = std::max(source.mip_level_count(), 1u);

    std::vector<vk::BufferImageCopy> regions;
    regions.reserve(mip_level_count);
    for (const uint32_t i : std::views::iota(0u, mip_level_count)) {
        const vk::DeviceSize buffer_offset{ source.offset_of(i, 0, 0) };

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

[[nodiscard]]
static auto create_staging_buffer(
    const core::renderer::base::Allocator& allocator,
    const std::span<const std::byte>       data
) -> core::renderer::resources::SeqWriteBuffer<>
{
    assert(!data.empty() && "image must not be empty");

    const vk::BufferCreateInfo staging_buffer_create_info{
        .size  = data.size_bytes(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return core::renderer::resources::SeqWriteBuffer<>{ allocator,
                                                        staging_buffer_create_info,
                                                        data.data() };
}

core::gfx::resources::Image::Loader::Loader(
    const vk::Device                 device,
    const renderer::base::Allocator& allocator,
    const image::Image&              source
)
    : m_image{ ::create_image(allocator, source) },
      m_view{ ::create_image_view(
          device,
          m_image.get(),
          m_image.format(),
          m_image.mip_level_count()
      ) },
      m_copy_regions{ ::create_copy_regions(source) },
      m_staging_buffer{ ::create_staging_buffer(allocator, source.data()) },
      m_needs_mip_generation{ source.needs_mip_generation() }
{}

auto core::gfx::resources::Image::Loader::operator()(
    const vk::PhysicalDevice            physical_device,
    const vk::CommandBuffer             graphics_command_buffer,
    const renderer::base::Image::State& new_state
) && -> Image
{
    transition_image_layout(
        graphics_command_buffer,
        m_image,
        renderer::base::Image::State{
            vk::PipelineStageFlagBits::eTransfer,
            vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eTransferDstOptimal,
        }
    );

    graphics_command_buffer.copyBufferToImage(
        m_staging_buffer.get(), m_image.get(), m_image.layout(), m_copy_regions
    );

    if (m_needs_mip_generation) {
        renderer::base::generate_mipmaps(
            physical_device, graphics_command_buffer, m_image, new_state
        );
    }
    else {
        transition_image_layout(graphics_command_buffer, m_image, new_state);
    }

    return Image{ std::move(m_image), std::move(m_view) };
}

auto core::gfx::resources::Image::Loader::image() const
    -> const renderer::resources::Image&
{
    return m_image;
}

auto core::gfx::resources::Image::Loader::view() const -> vk::ImageView
{
    return m_view.get();
}

auto core::gfx::resources::Image::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& physical_device_selector
) -> void
{
    constexpr static vk::PhysicalDeviceFeatures features{
        .samplerAnisotropy = vk::True,
    };
    physical_device_selector.set_required_features(features);
}

auto core::gfx::resources::Image::view() const -> vk::ImageView
{
    return m_view.get();
}

core::gfx::resources::Image::Image(
    renderer::resources::Image&& image,
    vk::UniqueImageView&&        view
) noexcept
    : m_image{ std::move(image) },
      m_view{ std::move(view) }
{}
