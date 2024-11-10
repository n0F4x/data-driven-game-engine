#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

#include "Program.hpp"
#include "VertexLayout.hpp"

namespace core::renderer {

class GraphicsPipelineBuilder {
public:
    explicit GraphicsPipelineBuilder(Program program) noexcept;

    auto set_program(Program program) noexcept -> GraphicsPipelineBuilder&;
    auto add_vertex_layout(VertexLayout vertex_layout) -> GraphicsPipelineBuilder&;
    auto set_primitive_topology(vk::PrimitiveTopology primitive_topology) noexcept
        -> GraphicsPipelineBuilder&;
    auto set_cull_mode(vk::CullModeFlags cull_mode) noexcept -> GraphicsPipelineBuilder&;
    auto enable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto disable_blending() noexcept -> GraphicsPipelineBuilder&;
    auto set_layout(vk::PipelineLayout layout) noexcept -> GraphicsPipelineBuilder&;
    auto set_render_pass(vk::RenderPass render_pass) noexcept -> GraphicsPipelineBuilder&;

    [[nodiscard]]
    auto build(vk::Device device, const void* next_create_info_struct = nullptr) const
        -> vk::UniquePipeline;

private:
    Program                   m_program;
    std::vector<VertexLayout> m_vertex_layouts;
    vk::PrimitiveTopology m_primitive_topology{ vk::PrimitiveTopology::eTriangleList };
    vk::CullModeFlags     m_cull_mode;
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
    auto operator()(
        const core::renderer::GraphicsPipelineBuilder& graphics_pipeline_builder
    ) const noexcept -> size_t;
};
