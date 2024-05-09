#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

#include "core/cache/Cache.hpp"
#include "core/cache/Handle.hpp"

#include "Effect.hpp"
#include "GraphicsPipeline.hpp"

namespace core::renderer {

class GraphicsPipelineBuilder {
public:
    explicit GraphicsPipelineBuilder(
        Effect                                              t_effect,
        std::optional<std::reference_wrapper<cache::Cache>> t_cache = {}
    ) noexcept;

    auto set_effect(Effect t_effect) noexcept -> GraphicsPipelineBuilder&;
    auto set_primitive_topology(vk::PrimitiveTopology t_primitive_topology
    ) noexcept -> GraphicsPipelineBuilder&;
    auto set_cull_mode(vk::CullModeFlags t_cull_mode) noexcept -> GraphicsPipelineBuilder&;
    auto enable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto disable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto set_layout(vk::PipelineLayout t_layout) noexcept -> GraphicsPipelineBuilder&;
    auto set_render_pass(vk::RenderPass t_render_pass
    ) noexcept -> GraphicsPipelineBuilder&;

    [[nodiscard]]
    auto build(vk::Device t_device) const -> vk::UniquePipeline;

private:
    std::optional<std::reference_wrapper<cache::Cache>> m_cache;
    Effect                                              m_effect;
    vk::PrimitiveTopology m_primitive_topology{ vk::PrimitiveTopology::eTriangleList };
    vk::CullModeFlags     m_cull_mode{};
    bool                  m_enable_blending{};
    vk::PipelineLayout    m_layout;
    vk::RenderPass        m_render_pass;

    friend auto hash_value(const GraphicsPipelineBuilder& t_graphics_pipeline_builder
    ) noexcept -> size_t;
};

}   // namespace core::renderer

template <>
struct std::hash<core::renderer::GraphicsPipelineBuilder> {
    [[nodiscard]]
    auto operator()(
        const core::renderer::GraphicsPipelineBuilder& t_graphics_pipeline_builder
    ) const noexcept -> size_t;
};
