module;

export module modules.gfx.resources.image_helpers;

import vulkan_hpp;

import modules.renderer.base.resources.Image;

namespace modules::gfx::resources {

export auto transition_image_layout(
    vk::CommandBuffer                   command_buffer,
    renderer::base::Image&              image,
    const renderer::base::Image::State& new_state
) -> renderer::base::Image::State;

}   // namespace modules::gfx::resources
