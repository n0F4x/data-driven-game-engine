#pragma once

#include <vulkan/vulkan.hpp>

#include "Image.hpp"

namespace core::renderer::base {

class Image;

auto generate_mipmaps(
    vk::PhysicalDevice  physical_device,
    vk::CommandBuffer   graphics_command_buffer,
    Image&              image,
    const Image::State& new_state
) -> void;

}   // namespace core::renderer::base
