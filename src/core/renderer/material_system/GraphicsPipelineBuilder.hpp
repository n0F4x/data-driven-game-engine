#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

#include "core/cache/Cache.hpp"
#include "core/cache/Handle.hpp"

#include "Effect.hpp"

namespace core::renderer {

class GraphicsPipelineBuilder {
public:
    explicit GraphicsPipelineBuilder(
        Effect                                              effect,
        std::optional<std::reference_wrapper<cache::Cache>> cache = {}
    ) noexcept;

    auto set_effect(Effect effect) noexcept -> GraphicsPipelineBuilder&;
    auto set_primitive_topology(vk::PrimitiveTopology primitive_topology
    ) noexcept -> GraphicsPipelineBuilder&;
    auto set_cull_mode(vk::CullModeFlags cull_mode) noexcept -> GraphicsPipelineBuilder&;
    auto enable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto disable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto set_layout(vk::PipelineLayout layout) noexcept -> GraphicsPipelineBuilder&;
    auto set_render_pass(vk::RenderPass render_pass) noexcept -> GraphicsPipelineBuilder&;

    [[nodiscard]]
    auto build(vk::Device device) const -> vk::UniquePipeline;

private:
    std::optional<std::reference_wrapper<cache::Cache>> m_cache;
    Effect                                              m_effect;
    vk::PrimitiveTopology m_primitive_topology{ vk::PrimitiveTopology::eTriangleList };
    vk::CullModeFlags     m_cull_mode{};
    bool                  m_enable_blending{};
    vk::PipelineLayout    m_layout;
    vk::RenderPass        m_render_pass;

    friend auto hash_value(const GraphicsPipelineBuilder& graphics_pipeline_builder
    ) noexcept -> size_t;
};

}   // namespace core::renderer

template <>
struct std::hash<core::renderer::GraphicsPipelineBuilder> {
    [[nodiscard]]
    auto operator()(const core::renderer::GraphicsPipelineBuilder& graphics_pipeline_builder
    ) const noexcept -> size_t;
};
