#pragma once

#include <array>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "engine/common/Cache.hpp"
#include "engine/common/Handle.hpp"

#include "Effect.hpp"

namespace engine::renderer {

class GraphicsPipelineBuilder {
public:
    explicit GraphicsPipelineBuilder(
        vk::Device           t_device,
        Effect               t_effect,
        tl::optional<Cache&> t_cache = {}
    ) noexcept;

    auto set_effect(Effect t_effect) noexcept -> GraphicsPipelineBuilder&;
    auto set_vertex_input_state(
        const vk::PipelineVertexInputStateCreateInfo* t_vertex_input_state
    ) noexcept -> GraphicsPipelineBuilder&;
    auto set_primitive_topology(vk::PrimitiveTopology t_primitive_topology) noexcept
        -> GraphicsPipelineBuilder&;
    auto set_cull_mode(vk::CullModeFlags t_cull_mode) noexcept
        -> GraphicsPipelineBuilder&;
    auto set_color_blend_state(
        const vk::PipelineColorBlendStateCreateInfo* t_color_blend_state
    ) noexcept -> GraphicsPipelineBuilder&;
    auto set_layout(vk::PipelineLayout t_layout) noexcept -> GraphicsPipelineBuilder&;
    auto set_render_pass(vk::RenderPass t_render_pass) noexcept
        -> GraphicsPipelineBuilder&;

    [[nodiscard]] auto build() const -> Handle<vk::UniquePipeline>;

private:
    vk::Device                                    m_device;
    tl::optional<std::reference_wrapper<Cache>>   m_cache;
    Effect                                        m_effect;
    const vk::PipelineVertexInputStateCreateInfo* m_vertex_input_state{};
    vk::PrimitiveTopology                         m_primitive_topology{};
    vk::CullModeFlags                             m_cull_mode{};
    const vk::PipelineColorBlendStateCreateInfo*  m_color_blend_state{};
    vk::PipelineLayout                            m_layout;
    vk::RenderPass                                m_render_pass;

    friend auto hash_value(const GraphicsPipelineBuilder& t_graphics_pipeline_builder
    ) noexcept -> size_t;
};

}   // namespace engine::renderer
