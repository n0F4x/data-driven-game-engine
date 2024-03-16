#pragma once

#include <vulkan/vulkan.hpp>

namespace core::renderer {

class Scene {
public:
    auto draw(vk::CommandBuffer t_graphics_command_buffer) const -> void;
};

}   // namespace core::renderer
