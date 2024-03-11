#include "GraphicsPipelineBuilder.hpp"

namespace engine::renderer {

GraphicsPipelineBuilder::GraphicsPipelineBuilder(
    vk::Device           t_device,
    Effect               t_effect,
    tl::optional<Cache&> t_cache
) noexcept
    : m_device{ t_device },
      m_cache{ t_cache },
      m_effect{ std::move(t_effect) }
{}

auto GraphicsPipelineBuilder::set_effect(Effect t_effect) noexcept
    -> GraphicsPipelineBuilder&
{
    m_effect = std::move(t_effect);
    return *this;
}

auto GraphicsPipelineBuilder::set_vertex_input_state(
    const vk::PipelineVertexInputStateCreateInfo* t_vertex_input
) noexcept -> GraphicsPipelineBuilder&
{
    m_create_info.pVertexInputState = t_vertex_input;
    return *this;
}

auto GraphicsPipelineBuilder::set_primitive_topology(
    const vk::PrimitiveTopology t_primitive_topology
) noexcept -> GraphicsPipelineBuilder&
{
    m_primitive_topology = t_primitive_topology;
    return *this;
}

auto GraphicsPipelineBuilder::set_cull_mode(const vk::CullModeFlags t_cull_mode) noexcept
    -> GraphicsPipelineBuilder&
{
    m_cull_mode = t_cull_mode;
    return *this;
}

auto GraphicsPipelineBuilder::set_color_blend_state(
    const vk::PipelineColorBlendStateCreateInfo* t_color_blend_state
) noexcept -> GraphicsPipelineBuilder&
{
    m_create_info.pColorBlendState = t_color_blend_state;
    return *this;
}

auto GraphicsPipelineBuilder::set_layout(const vk::PipelineLayout t_layout) noexcept
    -> GraphicsPipelineBuilder&
{
    m_create_info.layout = t_layout;
    return *this;
}

auto GraphicsPipelineBuilder::set_render_pass(const vk::RenderPass t_render_pass) noexcept
    -> GraphicsPipelineBuilder&
{
    m_create_info.renderPass = t_render_pass;
    return *this;
}

// auto GraphicsPipelineBuilder::build() -> Handle<vk::UniquePipeline> {}

//[[nodiscard]] auto hash_value(const GraphicsPipelineBuilder& t_graphics_pipeline_builder
//) noexcept -> size_t
//{}

}   // namespace engine::renderer
