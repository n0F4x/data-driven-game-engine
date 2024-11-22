module;

#include <vulkan/vulkan.hpp>

export module core.renderer.base.resources.mipmap_generation;

import core.renderer.base.resources.Image;

namespace core::renderer::base {

export auto generate_mipmaps(
    vk::PhysicalDevice  physical_device,
    vk::CommandBuffer   graphics_command_buffer,
    Image&              image,
    const Image::State& new_state
) -> void;

}   // namespace core::renderer::base
