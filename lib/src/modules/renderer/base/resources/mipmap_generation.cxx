module;

#include <ranges>


module ddge.modules.renderer.base.resources.mipmap_generation;

import ddge.modules.renderer.base.resources.Image;

auto ddge::renderer::base::generate_mipmaps(
    const vk::PhysicalDevice physical_device,
    const vk::CommandBuffer  graphics_command_buffer,
    Image&                   image,
    const Image::State&      new_state
) -> void
{
    if (!(physical_device.getFormatProperties(image.format()).optimalTilingFeatures
          & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        throw std::runtime_error("Texture image format does not support linear blitting");
    }

    {
        const Image::State old_state{ image.transition(new_state) };

        const std::array pre_barriers{
            vk::ImageMemoryBarrier{
                                   .srcAccessMask       = old_state.access_mask,
                                   .dstAccessMask       = vk::AccessFlagBits::eTransferRead,
                                   .oldLayout           = old_state.layout,
                                   .newLayout           = vk::ImageLayout::eTransferSrcOptimal,
                                   .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .image               = image.get(),
                                   .subresourceRange =
                                   vk::ImageSubresourceRange{
                                   .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                   .baseMipLevel   = 0,
                                   .levelCount     = 1,
                                   .baseArrayLayer = 0,
                                   .layerCount     = 1,
                                   }, },
            vk::ImageMemoryBarrier{
                                   .srcAccessMask       = old_state.access_mask,
                                   .dstAccessMask       = vk::AccessFlagBits::eTransferWrite,
                                   .oldLayout           = old_state.layout,
                                   .newLayout           = vk::ImageLayout::eTransferDstOptimal,
                                   .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .image               = image.get(),
                                   .subresourceRange =
                                   vk::ImageSubresourceRange{
                                   .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                   .baseMipLevel   = 1,
                                   .levelCount     = image.mip_level_count() - 1,
                                   .baseArrayLayer = 0,
                                   .layerCount     = 1,
                                   }, },
        };

        graphics_command_buffer.pipelineBarrier(
            old_state.stage_mask,
            vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlags{},
            {},
            {},
            pre_barriers
        );
    }

    vk::ImageMemoryBarrier barrier{
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image.get(),
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };

    auto mip_width{ static_cast<int32_t>(image.extent().width) };
    auto mip_height{ static_cast<int32_t>(image.extent().height) };
    for (const uint32_t blitted_mip_level_index :
         std::views::iota(1u, image.mip_level_count()))
    {
        if (blitted_mip_level_index != 1u) {
            barrier.subresourceRange.baseMipLevel = blitted_mip_level_index - 1;
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
        }

        const std::array src_offsets{
            vk::Offset3D{         0,          0, 0 },
            vk::Offset3D{ mip_width, mip_height, 1 },
        };
        const vk::ImageSubresourceLayers src_subresource{
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .mipLevel       = blitted_mip_level_index - 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };
        const std::array dst_offsets{
            vk::Offset3D{ 0, 0, 0 },
            vk::Offset3D{
                         .x = mip_width > 1 ? mip_width / 2 : 1,
                         .y = mip_height > 1 ? mip_height / 2 : 1,
                         .z = 1,
                         },
        };
        const vk::ImageSubresourceLayers dst_subresource{
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .mipLevel       = blitted_mip_level_index,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };

        const vk::ImageBlit blit{
            .srcSubresource = src_subresource,
            .srcOffsets     = src_offsets,
            .dstSubresource = dst_subresource,
            .dstOffsets     = dst_offsets,
        };
        graphics_command_buffer.blitImage(
            image.get(),
            vk::ImageLayout::eTransferSrcOptimal,
            image.get(),
            vk::ImageLayout::eTransferDstOptimal,
            blit,
            vk::Filter::eLinear
        );

        if (mip_width > 1) {
            mip_width /= 2;
        }
        if (mip_height > 1) {
            mip_height /= 2;
        }
    }

    {
        const std::array after_barriers{
            vk::ImageMemoryBarrier{
                                   .srcAccessMask       = vk::AccessFlagBits::eTransferRead,
                                   .dstAccessMask       = new_state.access_mask,
                                   .oldLayout           = vk::ImageLayout::eTransferSrcOptimal,
                                   .newLayout           = new_state.layout,
                                   .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .image               = image.get(),
                                   .subresourceRange =
                                   vk::ImageSubresourceRange{
                                   .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                   .baseMipLevel   = 0,
                                   .levelCount     = image.mip_level_count() - 1,
                                   .baseArrayLayer = 0,
                                   .layerCount     = 1,
                                   }, },
            vk::ImageMemoryBarrier{
                                   .srcAccessMask       = vk::AccessFlagBits::eTransferWrite,
                                   .dstAccessMask       = new_state.access_mask,
                                   .oldLayout           = vk::ImageLayout::eTransferDstOptimal,
                                   .newLayout           = new_state.layout,
                                   .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                   .image               = image.get(),
                                   .subresourceRange =
                                   vk::ImageSubresourceRange{
                                   .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                   .baseMipLevel   = image.mip_level_count() - 1,
                                   .levelCount     = 1,
                                   .baseArrayLayer = 0,
                                   .layerCount     = 1,
                                   }, },
        };

        graphics_command_buffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            new_state.stage_mask,
            vk::DependencyFlags{},
            {},
            {},
            after_barriers
        );
    }
}
