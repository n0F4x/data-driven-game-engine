#pragma once

#include <array>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "core/common/Cache.hpp"
#include "core/common/Handle.hpp"

#include "Effect.hpp"
#include "GraphicsPipeline.hpp"
#include "VertexInputState.hpp"
#include "VertexInputStateBuilder.hpp"

namespace core::renderer {

class GraphicsPipelineBuilder {
public:
    explicit GraphicsPipelineBuilder(
        vk::Device           t_device,
        Effect               t_effect,
        tl::optional<Cache&> t_cache = {}
    ) noexcept;

    auto set_effect(Effect t_effect) noexcept -> GraphicsPipelineBuilder&;
    auto set_vertex_input_state(const VertexInputStateBuilder& t_vertex_input_state_builder
    ) noexcept -> GraphicsPipelineBuilder&;
    auto set_primitive_topology(vk::PrimitiveTopology t_primitive_topology) noexcept
        -> GraphicsPipelineBuilder&;
    auto set_cull_mode(vk::CullModeFlags t_cull_mode) noexcept
        -> GraphicsPipelineBuilder&;
    auto enable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto disable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto set_layout(vk::PipelineLayout t_layout) noexcept -> GraphicsPipelineBuilder&;
    auto set_render_pass(vk::RenderPass t_render_pass) noexcept
        -> GraphicsPipelineBuilder&;

    [[nodiscard]] auto build() const -> Handle<GraphicsPipeline>;

private:
    vk::Device                                  m_device;
    tl::optional<std::reference_wrapper<Cache>> m_cache;
    Effect                                      m_effect;
    tl::optional<VertexInputState>              m_vertex_input_state;
    vk::PrimitiveTopology m_primitive_topology{ vk::PrimitiveTopology::eTriangleList };
    vk::CullModeFlags     m_cull_mode{};
    bool                  m_enable_blending{};
    vk::PipelineLayout    m_layout;
    vk::RenderPass        m_render_pass;

    friend auto hash_value(const GraphicsPipelineBuilder& t_graphics_pipeline_builder
    ) noexcept -> size_t;
};

}   // namespace core::renderer

namespace std {

template <>
class hash<core::renderer::GraphicsPipelineBuilder> {
public:
    [[nodiscard]] auto operator()(
        const core::renderer::GraphicsPipelineBuilder& t_graphics_pipeline_builder
    ) const -> size_t;
};

}   // namespace std
