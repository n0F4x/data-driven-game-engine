#pragma once

#include <vulkan/vulkan.hpp>

class Material {
public:
    explicit Material(vk::Pipeline t_pipeline) noexcept;

    auto bind(vk::CommandBuffer t_graphics_command_buffer) const -> void;

private:
    vk::Pipeline m_pipeline;
};
