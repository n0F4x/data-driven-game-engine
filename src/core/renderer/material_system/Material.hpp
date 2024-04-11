#pragma once

#include <vulkan/vulkan.hpp>

#include "core/cache/Handle.hpp"

#include "GraphicsPipeline.hpp"

namespace core::renderer {

class Material {
public:
    explicit Material(cache::Handle<GraphicsPipeline> t_pipeline) noexcept;

    auto bind(vk::CommandBuffer t_graphics_command_buffer) const -> void;

private:
    cache::Handle<GraphicsPipeline> m_pipeline;
};

}   // namespace core::renderer
