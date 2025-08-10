module;

export module ddge.modules.renderer.base.resources.mipmap_generation;

import vulkan_hpp;

import ddge.modules.renderer.base.resources.Image;

namespace ddge::renderer::base {

export auto generate_mipmaps(
    vk::PhysicalDevice  physical_device,
    vk::CommandBuffer   graphics_command_buffer,
    Image&              image,
    const Image::State& new_state
) -> void;

}   // namespace ddge::renderer::base
