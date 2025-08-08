module;

export module modules.renderer.base.resources.mipmap_generation;

import vulkan_hpp;

import modules.renderer.base.resources.Image;

namespace modules::renderer::base {

export auto generate_mipmaps(
    vk::PhysicalDevice  physical_device,
    vk::CommandBuffer   graphics_command_buffer,
    Image&              image,
    const Image::State& new_state
) -> void;

}   // namespace modules::renderer::base
