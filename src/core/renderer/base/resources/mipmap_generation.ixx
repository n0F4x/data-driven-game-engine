module;

export module core.renderer.base.resources.mipmap_generation;

import vulkan_hpp;

import core.renderer.base.resources.Image;

namespace core::renderer::base {

export auto generate_mipmaps(
    vk::PhysicalDevice  physical_device,
    vk::CommandBuffer   graphics_command_buffer,
    Image&              image,
    const Image::State& new_state
) -> void;

}   // namespace core::renderer::base
