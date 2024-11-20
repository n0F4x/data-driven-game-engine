#include "image_helpers.hpp"

auto core::gfx::resources::transition_image_layout(
    const vk::CommandBuffer             command_buffer,
    renderer::base::Image&              image,
    const renderer::base::Image::State& new_state
) -> renderer::base::Image::State
{
    const auto old_state{ image.transition(new_state) };

    const vk::ImageMemoryBarrier barrier{
        .srcAccessMask       = old_state.access_mask,
        .dstAccessMask       = new_state.access_mask,
        .oldLayout           = old_state.layout,
        .newLayout           = new_state.layout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image.get(),
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask = image.aspect_flags(),
                                      .levelCount = image.mip_level_count(),
                                      .layerCount = 1u },
    };

    command_buffer.pipelineBarrier(
        old_state.stage_mask, new_state.stage_mask, vk::DependencyFlags{}, {}, {}, barrier
    );

    return old_state;
}
