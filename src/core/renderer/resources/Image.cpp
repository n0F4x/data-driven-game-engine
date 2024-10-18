#include "Image.hpp"

#include <ranges>

#include "core/renderer/base/allocator/Allocator.hpp"

[[nodiscard]]
static auto create_copy_regions(const core::image::Image& source)
    -> std::vector<vk::BufferImageCopy>
{
    const uint32_t mip_level_count = source.mip_level_count() == 0
                                       ? 1
                                       : source.mip_level_count();

    std::vector<vk::BufferImageCopy> regions;
    regions.reserve(mip_level_count);
    for (const uint32_t i : std::views::iota(0u, mip_level_count)) {
        const vk::DeviceSize buffer_offset{ source.offset_of(i, 0, 0) };

        vk::Extent3D image_extent{
            .width  = source.width() >> i,
            .height = source.height() >> i,
            .depth  = 1,
        };
        if (image_extent.width == 0) {
            image_extent.width = 1;
        }
        if (image_extent.height == 0) {
            image_extent.height = 1;
        }

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
static auto image_usage_flags(const bool needs_to_blit_mipmaps)
{
    vk::ImageUsageFlags result{ vk::ImageUsageFlagBits::eTransferDst
                                | vk::ImageUsageFlagBits::eSampled };

    if (needs_to_blit_mipmaps) {
        result |= vk::ImageUsageFlagBits::eTransferSrc;
    }

    return result;
}

[[nodiscard]]
static auto create_image(
    const core::renderer::base::Allocator& allocator,
    const vk::Format                       format,
    const vk::Extent3D&                    extent,
    const uint32_t                         mip_level_count,
    const vk::ImageUsageFlags              usage
) -> core::renderer::base::Image
{
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

    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    return allocator.allocate_image(image_create_info, allocation_create_info);
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
static auto create_staging_buffer(
    const core::renderer::base::Allocator& allocator,
    const std::span<const std::byte>       data
) -> core::renderer::base::SeqWriteBuffer<>
{
    if (data.empty()) {
        return core::renderer::base::SeqWriteBuffer<>{};
    }

    const vk::BufferCreateInfo staging_buffer_create_info{
        .size  = data.size_bytes(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return allocator.allocate_seq_write_buffer<>(staging_buffer_create_info, data.data());
}

static auto transition_image_layout(
    const vk::CommandBuffer command_buffer,
    const vk::Image         image,
    const vk::ImageLayout   old_layout,
    const vk::ImageLayout   new_layout,
    const uint32_t          mip_level_count
) -> void
{
    vk::ImageMemoryBarrier barrier{
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                      .baseMipLevel   = 0,
                                      .levelCount     = mip_level_count,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };
    vk::PipelineStageFlags source_stage;
    vk::PipelineStageFlags destination_stage;

    if (old_layout == vk::ImageLayout::eUndefined
        && new_layout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        source_stage      = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        source_stage      = vk::PipelineStageFlagBits::eTransfer;
        destination_stage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        assert(false && "Unsupported layout transition!");
    }

    command_buffer.pipelineBarrier(
        source_stage, destination_stage, vk::DependencyFlags{}, {}, {}, barrier
    );
}

static auto generate_mipmaps(
    const vk::PhysicalDevice physical_device,
    const vk::CommandBuffer  graphics_command_buffer,
    const vk::Image          image,
    const vk::Format         format,
    const int32_t            base_width,
    const int32_t            base_height,
    const uint32_t           mip_level_count
) -> void
{
    if (!(physical_device.getFormatProperties(format).optimalTilingFeatures
          & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        throw std::runtime_error("Texture image format does not support linear blitting!");
    }

    vk::ImageMemoryBarrier barrier{
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };

    int32_t mip_width  = base_width;
    int32_t mip_height = base_height;
    for (uint32_t i = 1; i < mip_level_count; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;

        graphics_command_buffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlags{},
            {},
            {},
            barrier
        );

        vk::ImageBlit blit{};
        blit.srcOffsets[0]  = vk::Offset3D{ 0, 0, 0 };
        blit.srcOffsets[1]  = vk::Offset3D{ mip_width, mip_height, 1 },
        blit.srcSubresource = vk::ImageSubresourceLayers{
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .mipLevel       = i - 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };
        blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
        blit.dstOffsets[1] = vk::Offset3D{
            .x = mip_width > 1 ? mip_width / 2 : 1,
            .y = mip_height > 1 ? mip_height / 2 : 1,
            .z = 1,
        };
        blit.dstSubresource = vk::ImageSubresourceLayers{
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .mipLevel       = i,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };

        graphics_command_buffer.blitImage(
            image,
            vk::ImageLayout::eTransferSrcOptimal,
            image,
            vk::ImageLayout::eTransferDstOptimal,
            blit,
            vk::Filter::eLinear
        );

        barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        graphics_command_buffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags{},
            {},
            {},
            barrier
        );

        if (mip_width > 1) {
            mip_width /= 2;
        }
        if (mip_height > 1) {
            mip_height /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = mip_level_count - 1;
    barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout                     = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;

    graphics_command_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::DependencyFlags{},
        {},
        {},
        barrier
    );
}

core::renderer::resources::Image::Loader::Loader(
    const vk::Device                 device,
    const renderer::base::Allocator& allocator,
    const core::image::Image&        source
)
    : m_format{ source.format() },
      m_extent{
          .width  = source.width(),
          .height = source.height(),
          .depth  = source.depth(),
      },
      m_mip_level_count{ source.mip_level_count() },
      m_copy_regions{ create_copy_regions(source) },
      m_image{ ::create_image(
          allocator,
          m_format,
          m_extent,
          mip_level_count(),
          image_usage_flags(needs_mip_generation())
      ) },
      m_view{ ::create_image_view(device, m_image.get(), m_format, mip_level_count()) },
      m_staging_buffer{ ::create_staging_buffer(allocator, source.data()) }
{}

auto core::renderer::resources::Image::Loader::operator()(
    const vk::PhysicalDevice physical_device,
    const vk::CommandBuffer  graphics_command_buffer
) && -> Image
{
    ::transition_image_layout(
        graphics_command_buffer,
        m_image.get(),
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        mip_level_count()
    );
    graphics_command_buffer.copyBufferToImage(
        m_staging_buffer.get(),
        m_image.get(),
        vk::ImageLayout::eTransferDstOptimal,
        m_copy_regions
    );
    if (needs_mip_generation()) {
        generate_mipmaps(
            physical_device,
            graphics_command_buffer,
            m_image.get(),
            m_format,
            static_cast<int32_t>(m_extent.width),
            static_cast<int32_t>(m_extent.height),
            mip_level_count()
        );
    }
    else {
        ::transition_image_layout(
            graphics_command_buffer,
            m_image.get(),
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            mip_level_count()
        );
    }


    return Image{ std::move(m_image), std::move(m_view) };
}

auto core::renderer::resources::Image::Loader::view() const -> vk::ImageView
{
    return m_view.get();
}

[[nodiscard]]
static auto count_mip_levels(const uint32_t base_width, const uint32_t base_height)
    -> uint32_t
{
    return static_cast<uint32_t>(std::floor(std::log2(std::max(base_width, base_height))))
         + 1;
}

auto core::renderer::resources::Image::Loader::mip_level_count() const -> uint32_t
{
    return m_mip_level_count == 0 ? count_mip_levels(m_extent.width, m_extent.height)
                                  : m_mip_level_count;
}

auto core::renderer::resources::Image::Loader::needs_mip_generation() const -> bool
{
    return m_mip_level_count == 0;
}

core::renderer::resources::Image::Image(base::Image&& image, vk::UniqueImageView&& view)
    noexcept
    : m_image{ std::move(image) },
      m_view{ std::move(view) }
{}
