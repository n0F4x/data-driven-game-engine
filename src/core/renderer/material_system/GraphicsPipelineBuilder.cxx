module;

#include <ranges>

#include <vulkan/vulkan_hash.hpp>

module core.renderer.material_system.GraphicsPipelineBuilder;

import utility.hashing;

core::renderer::GraphicsPipelineBuilder::GraphicsPipelineBuilder(Program program) noexcept
    : m_program{ std::move(program) }
{}

auto core::renderer::GraphicsPipelineBuilder::set_program(Program program) noexcept
    -> GraphicsPipelineBuilder&
{
    m_program = std::move(program);
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::add_vertex_layout(VertexLayout vertex_layout
) -> GraphicsPipelineBuilder&
{
    m_vertex_layouts.push_back(std::move(vertex_layout));
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::set_primitive_topology(
    const vk::PrimitiveTopology primitive_topology
) noexcept -> GraphicsPipelineBuilder&
{
    m_primitive_topology = primitive_topology;
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::set_cull_mode(
    const vk::CullModeFlags cull_mode
) noexcept -> GraphicsPipelineBuilder&
{
    m_cull_mode = cull_mode;
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::enable_blending() noexcept
    -> GraphicsPipelineBuilder&
{
    m_enable_blending = true;
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::disable_blending() noexcept
    -> GraphicsPipelineBuilder&
{
    m_enable_blending = false;
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::set_layout(const vk::PipelineLayout layout
) noexcept -> GraphicsPipelineBuilder&
{
    m_layout = layout;
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::set_render_pass(
    const vk::RenderPass render_pass
) noexcept -> GraphicsPipelineBuilder&
{
    m_render_pass = render_pass;
    return *this;
}

auto core::renderer::GraphicsPipelineBuilder::build(
    const vk::Device device,
    const void*      next_create_info_struct
) const -> vk::UniquePipeline
{
    std::array pipeline_stages{ m_program.pipeline_stages() };

    const std::vector<vk::VertexInputBindingDescription> vertex_input_binding_descriptions{
        std::views::zip(m_vertex_layouts, std::views::iota(0u, m_vertex_layouts.size()))
        | std::views::transform([](const std::pair<const VertexLayout&, uint32_t> pack) {
              auto [vertex_layout, index]{ pack };
              return vk::VertexInputBindingDescription{
                  .binding   = index,
                  .stride    = vertex_layout.stride(),
                  .inputRate = vertex_layout.input_rate(),
              };
          })
        | std::ranges::to<std::vector>()
    };
    const std::vector<vk::VertexInputAttributeDescription> vertex_input_attribute_descriptions{
        std::views::zip(m_vertex_layouts, std::views::iota(0u, m_vertex_layouts.size()))
        | std::views::transform([](const std::pair<const VertexLayout&, uint32_t> pack) {
              auto [vertex_layout, index]{ pack };
              return vertex_layout.attributes()
                   | std::views::transform([index](const VertexAttribute& attribute) {
                         return vk::VertexInputAttributeDescription{
                             .location = attribute.location,
                             .binding  = index,
                             .format   = attribute.format,
                             .offset   = attribute.offset,
                         };
                     });
          })
        | std::views::join | std::ranges::to<std::vector>()
    };
    const vk::PipelineVertexInputStateCreateInfo vertex_input_state_create_info{
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertex_input_binding_descriptions.size()),
        .pVertexBindingDescriptions = vertex_input_binding_descriptions.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertex_input_attribute_descriptions.size()),
        .pVertexAttributeDescriptions = vertex_input_attribute_descriptions.data(),
    };

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
        .depthTestEnable       = vk::True,
        .depthWriteEnable      = vk::True,
        .depthCompareOp        = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable     = vk::False,
    };

    vk::PipelineColorBlendAttachmentState color_blend_attachment_state{
        .blendEnable    = static_cast<vk::Bool32>(m_enable_blending),
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
    const vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info{
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
        .pNext               = next_create_info_struct,
        .stageCount          = static_cast<uint32_t>(pipeline_stages.size()),
        .pStages             = pipeline_stages.data(),
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
auto core::renderer::hash_value(const GraphicsPipelineBuilder& graphics_pipeline_builder
) noexcept -> size_t
{
    return ::core::hash_combine(
        graphics_pipeline_builder.m_program,
        graphics_pipeline_builder.m_primitive_topology,
        graphics_pipeline_builder.m_cull_mode,
        graphics_pipeline_builder.m_enable_blending,
        graphics_pipeline_builder.m_layout,
        graphics_pipeline_builder.m_render_pass
    );
}

auto std::hash<core::renderer::GraphicsPipelineBuilder>::operator()(
    const core::renderer::GraphicsPipelineBuilder& graphics_pipeline_builder
) const noexcept -> size_t
{
    return hash_value(graphics_pipeline_builder);
}
