#include "GraphicsPipelineBuilder.hpp"

#include <vulkan/vulkan_hash.hpp>

#include "core/utility/hashing.hpp"

namespace core::renderer {

GraphicsPipelineBuilder::GraphicsPipelineBuilder(
    Effect                                              effect,
    std::optional<std::reference_wrapper<cache::Cache>> cache
) noexcept
    : m_cache{ cache },
      m_effect{ std::move(effect) }
{}

auto GraphicsPipelineBuilder::set_effect(Effect effect
) noexcept -> GraphicsPipelineBuilder&
{
    m_effect = std::move(effect);
    return *this;
}

auto GraphicsPipelineBuilder::set_primitive_topology(
    const vk::PrimitiveTopology primitive_topology
) noexcept -> GraphicsPipelineBuilder&
{
    m_primitive_topology = primitive_topology;
    return *this;
}

auto GraphicsPipelineBuilder::set_cull_mode(const vk::CullModeFlags cull_mode
) noexcept -> GraphicsPipelineBuilder&
{
    m_cull_mode = cull_mode;
    return *this;
}

auto GraphicsPipelineBuilder::enable_blending() noexcept -> GraphicsPipelineBuilder&
{
    m_enable_blending = true;
    return *this;
}

auto GraphicsPipelineBuilder::disable_blending() noexcept -> GraphicsPipelineBuilder&
{
    m_enable_blending = false;
    return *this;
}

auto GraphicsPipelineBuilder::set_layout(const vk::PipelineLayout layout
) noexcept -> GraphicsPipelineBuilder&
{
    m_layout = layout;
    return *this;
}

auto GraphicsPipelineBuilder::set_render_pass(const vk::RenderPass render_pass
) noexcept -> GraphicsPipelineBuilder&
{
    m_render_pass = render_pass;
    return *this;
}

auto GraphicsPipelineBuilder::build(const vk::Device device) const -> vk::UniquePipeline
{
    // TODO: allow vertex input states
    constexpr static vk::PipelineVertexInputStateCreateInfo
        vertex_input_state_create_info{};

    const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{
        .topology = m_primitive_topology
    };

    constexpr vk::PipelineViewportStateCreateInfo viewport_state_create_info{
        .viewportCount = 1, .scissorCount = 1
    };

    const vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info{
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode    = m_cull_mode,
        .frontFace   = vk::FrontFace::eCounterClockwise,
        .lineWidth   = 1.f
    };

    constexpr vk::PipelineMultisampleStateCreateInfo multisample_state_create_info{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
    };

    constexpr vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{
        .depthTestEnable       = true,
        .depthWriteEnable      = true,
        .depthCompareOp        = vk::CompareOp::eLess,
        .depthBoundsTestEnable = false,
        .stencilTestEnable     = false,
    };

    vk::PipelineColorBlendAttachmentState color_blend_attachment_state{
        .blendEnable    = m_enable_blending,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                        | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };
    if (m_enable_blending) {
        color_blend_attachment_state.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        color_blend_attachment_state.dstColorBlendFactor =
            vk::BlendFactor::eOneMinusSrcAlpha;
        color_blend_attachment_state.colorBlendOp = vk::BlendOp::eAdd;
        color_blend_attachment_state.srcAlphaBlendFactor =
            vk::BlendFactor::eOneMinusSrcAlpha;
        color_blend_attachment_state.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        color_blend_attachment_state.alphaBlendOp        = vk::BlendOp::eAdd;
    }
    vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info{
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment_state,
    };

    constexpr static std::array dynamic_states{ vk::DynamicState::eViewport,
                                                vk::DynamicState::eScissor };
    constexpr vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data()
    };

    const vk::GraphicsPipelineCreateInfo create_info{
        .stageCount          = static_cast<uint32_t>(m_effect.pipeline_stages().size()),
        .pStages             = m_effect.pipeline_stages().data(),
        .pVertexInputState   = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pViewportState      = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState   = &multisample_state_create_info,
        .pDepthStencilState  = &depth_stencil_state_create_info,
        .pColorBlendState    = &color_blend_state_create_info,
        .pDynamicState       = &dynamic_state_create_info,
        .layout              = m_layout,
        .renderPass          = m_render_pass,
    };

    return device.createGraphicsPipelineUnique(nullptr, create_info).value;
}

[[nodiscard]]
auto hash_value(const GraphicsPipelineBuilder& graphics_pipeline_builder
) noexcept -> size_t
{
    return hash_combine(
        graphics_pipeline_builder.m_effect,
        graphics_pipeline_builder.m_primitive_topology,
        graphics_pipeline_builder.m_cull_mode,
        graphics_pipeline_builder.m_enable_blending,
        graphics_pipeline_builder.m_layout,
        graphics_pipeline_builder.m_render_pass
    );
}

}   // namespace core::renderer

auto std::hash<core::renderer::GraphicsPipelineBuilder>::operator()(
    const core::renderer::GraphicsPipelineBuilder& graphics_pipeline_builder
) const noexcept -> size_t
{
    return core::renderer::hash_value(graphics_pipeline_builder);
}
