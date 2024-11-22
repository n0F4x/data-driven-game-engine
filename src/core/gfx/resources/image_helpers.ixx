module;

#include <vulkan/vulkan.hpp>

export module core.gfx.resources.image_helpers;

import core.renderer.base.resources.Image;

namespace core::gfx::resources {

export auto transition_image_layout(
    vk::CommandBuffer                   command_buffer,
    renderer::base::Image&              image,
    const renderer::base::Image::State& new_state
) -> renderer::base::Image::State;

}   // namespace core::gfx::resources
