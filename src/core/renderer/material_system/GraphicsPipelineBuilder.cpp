#include "GraphicsPipelineBuilder.hpp"

#include <vulkan/vulkan_hash.hpp>

#include "core/utility/hashing.hpp"

namespace core::renderer {

GraphicsPipelineBuilder::GraphicsPipelineBuilder(
    vk::Device           t_device,
    Effect               t_effect,
    tl::optional<Cache&> t_cache
) noexcept
    : m_device{ t_device },
      m_cache{ t_cache.transform([](Cache& cache) { return std::ref(cache); }) },
      m_effect{ std::move(t_effect) }
{}

auto GraphicsPipelineBuilder::set_effect(Effect t_effect) noexcept
    -> GraphicsPipelineBuilder&
{
    m_effect = std::move(t_effect);
    return *this;
}

auto GraphicsPipelineBuilder::set_vertex_input_state(
    const VertexInputStateBuilder& t_vertex_input_state_builder
) noexcept -> GraphicsPipelineBuilder&
{
    m_vertex_input_state = t_vertex_input_state_builder.build(m_effect);
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

auto GraphicsPipelineBuilder::set_layout(const vk::PipelineLayout t_layout) noexcept
    -> GraphicsPipelineBuilder&
{
    m_layout = t_layout;
    return *this;
}

auto GraphicsPipelineBuilder::set_render_pass(const vk::RenderPass t_render_pass) noexcept
    -> GraphicsPipelineBuilder&
{
    m_render_pass = t_render_pass;
    return *this;
}

auto GraphicsPipelineBuilder::build() const -> Handle<vk::UniquePipeline>
{
    if (const auto cached{ m_cache.and_then([&](const Cache& cache) {
            return cache.find<vk::UniquePipeline>(hash_value(*this));
        }) };
        cached.has_value())
    {
        return cached.value();
    }

    const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{
        .topology = m_primitive_topology
    };

    const vk::PipelineViewportStateCreateInfo viewport_state_create_info{
        .viewportCount = 1, .scissorCount = 1
    };

    const vk::PipelineRasterizationStateCreateInfo rasterization_state_create_info{
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode    = m_cull_mode,
        .frontFace   = vk::FrontFace::eCounterClockwise,
        .lineWidth   = 1.f
    };

    const vk::PipelineMultisampleStateCreateInfo multisample_state_create_info{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
    };

    const vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{
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

    const std::array                         dynamic_states{ vk::DynamicState::eViewport,
                                     vk::DynamicState::eScissor };
    const vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data()
    };

    const vk::GraphicsPipelineCreateInfo create_info{
        .stageCount          = static_cast<uint32_t>(m_effect.pipeline_stages().size()),
        .pStages             = m_effect.pipeline_stages().data(),
        .pVertexInputState   = &m_vertex_input_state->info(),
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

    return m_cache
        .transform([&](Cache& cache) {
            return cache.emplace<vk::UniquePipeline>(
                hash_value(*this),
                m_device.createGraphicsPipelineUnique(nullptr, create_info).value
            );
        })
        .value_or(make_handle<vk::UniquePipeline>(
            m_device.createGraphicsPipelineUnique(nullptr, create_info).value
        ));
}

[[nodiscard]] auto hash_value(const GraphicsPipelineBuilder& t_graphics_pipeline_builder
) noexcept -> size_t
{
    return hash_combine(
        t_graphics_pipeline_builder.m_effect,
        t_graphics_pipeline_builder.m_vertex_input_state,
        t_graphics_pipeline_builder.m_primitive_topology,
        t_graphics_pipeline_builder.m_cull_mode,
        t_graphics_pipeline_builder.m_enable_blending,
        t_graphics_pipeline_builder.m_layout,
        t_graphics_pipeline_builder.m_render_pass
    );
}

}   // namespace core::renderer

namespace std {

auto hash<core::renderer::GraphicsPipelineBuilder>::operator()(
    const core::renderer::GraphicsPipelineBuilder& t_graphics_pipeline_builder
) const -> size_t
{
    return core::renderer::hash_value(t_graphics_pipeline_builder);
}

}   // namespace std
