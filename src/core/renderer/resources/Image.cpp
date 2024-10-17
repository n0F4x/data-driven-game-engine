#include "Image.hpp"

#include "core/renderer/base/allocator/Allocator.hpp"

[[nodiscard]]
static auto create_image(
    const core::renderer::base::Allocator& allocator,
    const core::image::Image&              source,
    const vk::ImageUsageFlags              usage
) -> core::renderer::base::Image
{
    const vk::ImageCreateInfo image_create_info{
        .imageType     = vk::ImageType::e2D,
        .format        = source.format(),
        .extent        = vk::Extent3D{ .width  = source.width(),
                                      .height = source.height(),
                                      .depth  = source.depth() },
        .mipLevels     = source.mip_level_count(),
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
    const vk::Format format
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo view_create_info{
        .image    = image,
        .viewType = vk::ImageViewType::e2D,
        .format   = format,
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                      .levelCount = 1,
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
    const vk::ImageLayout   new_layout
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
                                      .levelCount     = 1,
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
        source_stage,
        destination_stage,
        vk::DependencyFlags{},
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );
}

static auto copy_buffer_to_image(
    const vk::CommandBuffer command_buffer,
    const vk::Buffer        buffer,
    const vk::Image         image,
    const vk::DeviceSize    buffer_offset,
    const vk::Extent3D&     extent
) -> void
{
    // TODO account for mip-maps
    // TODO account for base layer
    const vk::BufferImageCopy region{
        .bufferOffset = buffer_offset,
        .imageSubresource =
            vk::ImageSubresourceLayers{ .aspectMask = vk::ImageAspectFlagBits::eColor,
                                       .layerCount = 1 },
        .imageExtent = extent,
    };

    command_buffer.copyBufferToImage(
        buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region
    );
}

core::renderer::resources::Image::Loader::Loader(
    const vk::Device                 device,
    const renderer::base::Allocator& allocator,
    const core::image::Image&        source
)
    : m_image{ ::create_image(
          allocator,
          source,
          vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
      ) },
      m_view{ ::create_image_view(device, m_image.get(), source.format()) },
      m_staging_buffer{ ::create_staging_buffer(allocator, source.data()) },
      m_extent{
          .width  = source.width(),
          .height = source.height(),
          .depth  = source.depth(),
      },
      m_buffer_offset{ source.offset_of(0, 0, 0) }
{}

auto core::renderer::resources::Image::Loader::operator()(
    const vk::CommandBuffer transfer_command_buffer
) && -> Image
{
    ::transition_image_layout(
        transfer_command_buffer,
        m_image.get(),
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );
    ::copy_buffer_to_image(
        transfer_command_buffer,
        m_staging_buffer.get(),
        m_image.get(),
        m_buffer_offset,
        m_extent
    );
    ::transition_image_layout(
        transfer_command_buffer,
        m_image.get(),
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    return Image{ std::move(m_image), std::move(m_view) };
}

auto core::renderer::resources::Image::Loader::view() const -> vk::ImageView
{
    return m_view.get();
}

core::renderer::resources::Image::Image(base::Image&& image, vk::UniqueImageView&& view)
    noexcept
    : m_image{ std::move(image) },
      m_view{ std::move(view) }
{}
