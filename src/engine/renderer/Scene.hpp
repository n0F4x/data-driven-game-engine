#pragma once

#include <vulkan/vulkan.hpp>

namespace engine::renderer {

class Scene {
public:
    auto draw(vk::CommandBuffer t_graphics_command_buffer) const -> void;
};

}   // namespace engine::renderer
