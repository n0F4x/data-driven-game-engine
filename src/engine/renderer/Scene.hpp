#pragma once

#include <vulkan/vulkan.hpp>

class Scene {
public:
    auto draw(vk::CommandBuffer t_graphics_command_buffer) const -> void;
};
