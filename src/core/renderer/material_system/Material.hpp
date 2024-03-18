#pragma once

#include <vulkan/vulkan.hpp>

#include "core/common/Handle.hpp"

#include "GraphicsPipeline.hpp"

namespace core::renderer {

class Material {
public:
    explicit Material(Handle<GraphicsPipeline> t_pipeline) noexcept;

    auto bind(vk::CommandBuffer t_graphics_command_buffer) const -> void;

private:
    Handle<GraphicsPipeline> m_pipeline;
};

}   // namespace core::renderer
