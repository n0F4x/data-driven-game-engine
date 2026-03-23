module;

#include <functional>
#include <utility>
#include <vector>

#include "util/contract_macros.hpp"
#include "util/lifetime_bound.hpp"

export module ddge.modules.vulkan.pipeline.GraphicsPipelineBuilder;

import vulkan_hpp;

import ddge.modules.vulkan.pipeline.ShaderModule;
import ddge.modules.vulkan.result.check_result;
import ddge.modules.vulkan.Device;
import ddge.util.contracts;

namespace ddge::vulkan {

export class GraphicsPipelineBuilder {
public:
    explicit GraphicsPipelineBuilder(
        [[lifetime_bound]] const ShaderModule&             vertex_shader,
        [[lifetime_bound]] const ShaderModule&             fragment_shader,
        [[lifetime_bound]] const vk::raii::PipelineLayout& pipeline_layout,
        vk::Format                                         image_format
    ) noexcept;

    template <typename Self_T>
    auto set_topology(this Self_T&&, vk::PrimitiveTopology primitive_topology) noexcept
        -> Self_T;

    [[nodiscard]]
    auto build(const Device& device) const -> vk::raii::Pipeline;

private:
    std::reference_wrapper<const ShaderModule> m_vertex_shader;
    std::reference_wrapper<const ShaderModule> m_fragment_shader;
    vk::PrimitiveTopology m_primitive_topology{ vk::PrimitiveTopology::eTriangleList };
    std::reference_wrapper<const vk::raii::PipelineLayout> m_layout;
    vk::Format                                             m_image_format;
};

}   // namespace ddge::vulkan

namespace ddge::vulkan {

template <typename Self_T>
auto GraphicsPipelineBuilder::set_topology(
    this Self_T&&               self,
    const vk::PrimitiveTopology primitive_topology
) noexcept -> Self_T
{
    self.m_primitive_topology = primitive_topology;
    return std::forward<Self_T>(self);
}

}   // namespace ddge::vulkan

module :private;

namespace ddge::vulkan {

GraphicsPipelineBuilder::GraphicsPipelineBuilder(
    const ShaderModule&             vertex_shader,
    const ShaderModule&             fragment_shader,
    const vk::raii::PipelineLayout& pipeline_layout,
    const vk::Format                image_format
) noexcept
    : m_vertex_shader{ vertex_shader },
      m_fragment_shader{ fragment_shader },
      m_layout{ pipeline_layout },
      m_image_format{ image_format }
{}

auto GraphicsPipelineBuilder::build(const Device& device) const -> vk::raii::Pipeline
{
    constexpr static auto has_required_features =
        [](const Device& x_device) static -> bool   //
    {
        return x_device.enabled_capabilities.contains_features(
            vk::PhysicalDeviceMaintenance5Features{ .maintenance5 = vk::True }
        );
    };
    PRECOND(has_required_features(device));

    const vk::ShaderModuleCreateInfo vertex_shader_module_create_info{
        .codeSize = m_vertex_shader.get().code().size_bytes(),
        .pCode    = m_vertex_shader.get().code().data(),
    };
    const vk::ShaderModuleCreateInfo fragment_shader_module_create_info{
        .codeSize = m_fragment_shader.get().code().size_bytes(),
        .pCode    = m_fragment_shader.get().code().data(),
    };

    const std::array shader_stage_create_infos{
        vk::PipelineShaderStageCreateInfo{
                                          .pNext = &vertex_shader_module_create_info,
                                          .stage = vk::ShaderStageFlagBits::eVertex,
                                          .pName = "main",
                                          },
        vk::PipelineShaderStageCreateInfo{
                                          .pNext = &fragment_shader_module_create_info,
                                          .stage = vk::ShaderStageFlagBits::eFragment,
                                          .pName = "main",
                                          },
    };

    constexpr static vk::PipelineVertexInputStateCreateInfo
        vertex_input_state_create_info{};

    const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{
        .topology = m_primitive_topology,
    };

    constexpr static vk::PipelineViewportStateCreateInfo viewport_state_create_info{
        .viewportCount = 1,
        .scissorCount  = 1,
    };

    constexpr static vk::PipelineRasterizationStateCreateInfo
        rasterization_state_create_info{
            .lineWidth = 1.f,
        };

    constexpr static vk::PipelineMultisampleStateCreateInfo multisample_state_create_info{
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
    };

    constexpr static vk::PipelineDepthStencilStateCreateInfo
        depth_stencil_state_create_info{
            .depthTestEnable       = vk::True,
            .depthWriteEnable      = vk::True,
            .depthCompareOp        = vk::CompareOp::eLess,
            .depthBoundsTestEnable = vk::False,
            .stencilTestEnable     = vk::False,
        };

    using enum vk::ColorComponentFlagBits;
    constexpr static vk::PipelineColorBlendAttachmentState color_blend_attachment_state{
        .colorWriteMask = eR | eG | eB | eA,
    };
    constexpr static vk::PipelineColorBlendStateCreateInfo color_blend_state_create_info{
        .attachmentCount = 1,
        .pAttachments    = &color_blend_attachment_state,
    };

    constexpr static std::array dynamic_states{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    constexpr static vk::PipelineDynamicStateCreateInfo dynamic_state_create_info{
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data(),
    };

    const vk::PipelineRenderingCreateInfo rendering_create_info{
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &m_image_format,
    };

    const vk::GraphicsPipelineCreateInfo create_info{
        .pNext               = &rendering_create_info,
        .stageCount          = static_cast<uint32_t>(shader_stage_create_infos.size()),
        .pStages             = shader_stage_create_infos.data(),
        .pVertexInputState   = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pViewportState      = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState   = &multisample_state_create_info,
        .pDepthStencilState  = &depth_stencil_state_create_info,
        .pColorBlendState    = &color_blend_state_create_info,
        .pDynamicState       = &dynamic_state_create_info,
        .layout              = m_layout.get(),
    };

    return vulkan::check_result(
        device.logical_device.createGraphicsPipeline(nullptr, create_info)
    );
}

}   // namespace ddge::vulkan
