#include "demo_init.hpp"

#include <bit>

#include <vulkan/vulkan.hpp>

#include <engine/asset_manager/GltfLoader.hpp>
#include <engine/graphics/Model.hpp>
#include <engine/graphics/ModelFactory.hpp>
#include <engine/utility/vulkan/tools.hpp>


using namespace engine;

namespace {

[[nodiscard]] auto find_supported_format(
    vk::PhysicalDevice             t_physical_device,
    const std::vector<vk::Format>& t_candidates,
    vk::ImageTiling                t_tiling,
    vk::FormatFeatureFlags         t_features
) -> tl::optional<vk::Format>
{
    for (auto format : t_candidates) {
        vk::FormatProperties format_properties;
        t_physical_device.getFormatProperties(format, &format_properties);

        if ((t_tiling == vk::ImageTiling::eLinear
             && (format_properties.linearTilingFeatures & t_features)
                    == t_features)
            || (t_tiling == vk::ImageTiling::eOptimal
                && (format_properties.optimalTilingFeatures & t_features)
                       == t_features))
        {
            return format;
        }
    }
    return tl::nullopt;
}

[[nodiscard]] auto find_depth_format(vk::PhysicalDevice t_physical_device
) noexcept -> tl::optional<vk::Format>
{
    using enum vk::Format;
    return find_supported_format(
        t_physical_device,
        { eD32Sfloat, eD32SfloatS8Uint, eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

}   // namespace

namespace init {

[[nodiscard]] auto create_render_pass(
    const vk::SurfaceFormatKHR& t_surface_format,
    const renderer::Device&     t_device
) noexcept -> vulkan::RenderPass
{
    vk::AttachmentDescription color_attachment_description{
        .format         = t_surface_format.format,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR
    };
    vk::AttachmentReference color_attachment_reference{
        .attachment = 0,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    };

    auto depth_format{ find_depth_format(t_device.physical_device()) };
    if (!depth_format) {
        return vulkan::RenderPass{ nullptr, nullptr };
    }
    vk::AttachmentDescription depth_attachment_description{
        .format         = *depth_format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
    vk::AttachmentReference depth_attachment_ref{
        .attachment = 1,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    std::array attachment_descriptions{ color_attachment_description,
                                        depth_attachment_description };

    vk::SubpassDescription subpass_description{
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &color_attachment_reference,
        .pDepthStencilAttachment = &depth_attachment_ref,
    };

    vk::SubpassDependency subpass_dependency{
        .srcSubpass   = VK_SUBPASS_EXTERNAL,
        .dstSubpass   = 0,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                      | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                      | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask = vk::AccessFlagBits::eNone,
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
                       | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
    };

    vk::RenderPassCreateInfo render_pass_create_info{
        .attachmentCount =
            static_cast<uint32_t>(attachment_descriptions.size()),
        .pAttachments    = attachment_descriptions.data(),
        .subpassCount    = 1,
        .pSubpasses      = &subpass_description,
        .dependencyCount = 1,
        .pDependencies   = &subpass_dependency,
    };

    return vulkan::RenderPass{
        *t_device, t_device->createRenderPass(render_pass_create_info).value
    };
}

auto create_depth_image(
    const renderer::Device& t_device,
    vk::Extent2D            t_swap_chain_extent
) noexcept -> vulkan::VmaImage
{
    auto opt_depth_format = find_depth_format(t_device.physical_device());
    if (!opt_depth_format) {
        return vulkan::VmaImage{ nullptr, nullptr, nullptr };
    }
    vk::Format depth_format{ *opt_depth_format };

    vk::ImageCreateInfo image_create_info = {
        .imageType = vk::ImageType::e2D,
        .format    = depth_format,
        .extent =
            vk::Extent3D{
                         .width  = t_swap_chain_extent.width,
                         .height = t_swap_chain_extent.height,
                         .depth  = 1,
                         },
        .mipLevels     = 1,
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = vk::ImageTiling::eOptimal,
        .usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    vk::Image     image{};
    VmaAllocation allocation;
    vmaCreateImage(
        t_device.allocator(),
        reinterpret_cast<const VkImageCreateInfo*>(&image_create_info),
        &allocation_create_info,
        reinterpret_cast<VkImage*>(&image),
        &allocation,
        nullptr
    );
    return engine::vulkan::VmaImage(t_device.allocator(), image, allocation);
}

auto create_depth_image_view(
    const renderer::Device& t_device,
    vk::Image               t_depth_image
) noexcept -> engine::vulkan::ImageView
{
    vk::ImageViewCreateInfo image_view_create_info{
        .image    = t_depth_image,
        .viewType = vk::ImageViewType::e2D,
        .format   = find_depth_format(t_device.physical_device()).value(),
        .subresourceRange =
            vk::ImageSubresourceRange{
                                      .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1,
                                      },
    };

    return engine::vulkan::ImageView(
        *t_device,
        t_device->createImageView(image_view_create_info, nullptr).value
    );
}

auto create_framebuffers(
    vk::Device                        t_device,
    vk::Extent2D                      t_swapchain_extent,
    const std::vector<vk::ImageView>& t_swapchain_image_views,
    vk::RenderPass                    t_render_pass,
    vk::ImageView                     t_depth_image_view
) noexcept -> std::vector<engine::vulkan::Framebuffer>
{
    std::vector<vulkan::Framebuffer> framebuffers;
    framebuffers.reserve(t_swapchain_image_views.size());

    for (auto swapchain_image_view : t_swapchain_image_views) {
        std::array attachments{ swapchain_image_view, t_depth_image_view };

        vk::FramebufferCreateInfo framebuffer_create_info{
            .renderPass      = t_render_pass,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments    = attachments.data(),
            .width           = t_swapchain_extent.width,
            .height          = t_swapchain_extent.height,
            .layers          = 1
        };

        if (!*framebuffers.emplace_back(
                t_device,
                t_device.createFramebuffer(framebuffer_create_info).value
            ))
        {
            return {};
        }
    }

    return framebuffers;
}

[[nodiscard]] auto create_descriptor_set_layout(vk::Device t_device) noexcept
    -> vulkan::DescriptorSetLayout
{
    vk::DescriptorSetLayoutBinding descriptor_set_layout_binding{
        .binding         = 0,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
        .stageFlags      = vk::ShaderStageFlagBits::eVertex
    };
    vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{
        .bindingCount = 1, .pBindings = &descriptor_set_layout_binding
    };

    return vulkan::DescriptorSetLayout{
        t_device,
        t_device.createDescriptorSetLayout(descriptor_set_layout_create_info)
            .value
    };
}

[[nodiscard]] auto create_pipeline_layout(
    vk::Device              t_device,
    vk::DescriptorSetLayout t_descriptor_set_layout,
    uint32_t                t_push_constant_size
) -> vulkan::PipelineLayout
{
    vk::PushConstantRange push_constant_range{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .size       = t_push_constant_size,
    };
    vk::PipelineLayoutCreateInfo pipeline_layout_create_info{
        .setLayoutCount         = 1,
        .pSetLayouts            = &t_descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &push_constant_range,
    };

    return vulkan::PipelineLayout{
        t_device,
        t_device.createPipelineLayout(pipeline_layout_create_info).value
    };
}

[[nodiscard]] auto create_pipeline(
    vk::Device         t_device,
    vk::PipelineLayout t_pipeline_layout,
    vk::RenderPass     t_render_pass
) -> vulkan::Pipeline
{
    const auto vertex_shader_module{
        vulkan::load_shader(t_device, "shaders/model_test.vert.spv")
    };
    const auto fragment_shader_module{
        vulkan::load_shader(t_device, "shaders/model_test.frag.spv")
    };
    if (!vertex_shader_module || !fragment_shader_module) {
        return vulkan::Pipeline{ nullptr, nullptr };
    }

    std::array pipeline_shader_stage_create_infos{
        vk::PipelineShaderStageCreateInfo{.stage =
vk::ShaderStageFlagBits::eVertex,
                                          .module = **vertex_shader_module,
                                          .pName  = "main" },
        vk::PipelineShaderStageCreateInfo{
                                          .stage  = vk::ShaderStageFlagBits::eFragment,
                                          .module = **fragment_shader_module,
                                          .pName  = "main" }
    };

    using Vertex = engine::gfx::Model::Vertex;
    vk::VertexInputBindingDescription vertex_input_binding_description{
        .binding   = 0,
        .stride    = sizeof(Vertex),
        .inputRate = vk::VertexInputRate::eVertex
    };
    std::array vertex_input_attribute_descriptions{
        vk::VertexInputAttributeDescription{
                                            .location = 0,
                                            .binding  = 0,
                                            .format   = vk::Format::eR32G32B32Sfloat,
                                            .offset   = static_cast<uint32_t>(offsetof(Vertex, position)) },
        vk::VertexInputAttributeDescription{
                                            .location = 1,
                                            .binding  = 0,
                                            .format   = vk::Format::eR32G32B32Sfloat,
                                            .offset   = static_cast<uint32_t>(offsetof(Vertex,    color)) },
        vk::VertexInputAttributeDescription{
                                            .location = 2,
                                            .binding  = 0,
                                            .format   = vk::Format::eR32G32B32Sfloat,
                                            .offset   = static_cast<uint32_t>(offsetof(Vertex,   normal)) }
    };

    vk::PipelineVertexInputStateCreateInfo
        pipeline_vertex_input_state_create_info{
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions    = &vertex_input_binding_description,
            .vertexAttributeDescriptionCount =
                static_cast<uint32_t>(vertex_input_attribute_descriptions.size()
                ),
            .pVertexAttributeDescriptions =
                vertex_input_attribute_descriptions.data()
        };

    vk::PipelineInputAssemblyStateCreateInfo
        pipeline_input_assembly_state_create_info{
            .topology = vk::PrimitiveTopology::eTriangleList
        };

    vk::PipelineViewportStateCreateInfo pipeline_viewport_state_create_info{
        .viewportCount = 1, .scissorCount = 1
    };

    vk::PipelineRasterizationStateCreateInfo
        pipeline_rasterization_state_create_info{
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode    = vk::CullModeFlagBits::eBack,
            .frontFace   = vk::FrontFace::eCounterClockwise,
            .lineWidth   = 1.f
        };

    vk::PipelineMultisampleStateCreateInfo
        pipeline_multisample_state_create_info{
            .rasterizationSamples = vk::SampleCountFlagBits::e1,
        };

    vk::PipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{
        .depthTestEnable       = true,
        .depthWriteEnable      = true,
        .depthCompareOp        = vk::CompareOp::eLess,
        .depthBoundsTestEnable = false,
        .stencilTestEnable     = false,
    };

    vk::PipelineColorBlendAttachmentState pipeline_color_blend_attachment_state{
        .blendEnable = false,
        .colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
            | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };
    vk::PipelineColorBlendStateCreateInfo
        pipeline_color_blend_state_create_info{
            .attachmentCount = 1,
            .pAttachments    = &pipeline_color_blend_attachment_state,
        };

    std::array dynamic_states{ vk::DynamicState::eViewport,
                               vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data()
    };

    vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info{
        .stageCount =
            static_cast<uint32_t>(pipeline_shader_stage_create_infos.size()),
        .pStages             = pipeline_shader_stage_create_infos.data(),
        .pVertexInputState   = &pipeline_vertex_input_state_create_info,
        .pInputAssemblyState = &pipeline_input_assembly_state_create_info,
        .pViewportState      = &pipeline_viewport_state_create_info,
        .pRasterizationState = &pipeline_rasterization_state_create_info,
        .pMultisampleState   = &pipeline_multisample_state_create_info,
        .pDepthStencilState  = &depth_stencil_state_create_info,
        .pColorBlendState    = &pipeline_color_blend_state_create_info,
        .pDynamicState       = &pipelineDynamicStateCreateInfo,
        .layout              = t_pipeline_layout,
        .renderPass          = t_render_pass,
    };

    return vulkan::Pipeline{
        t_device,
        t_device.createGraphicsPipeline(nullptr, graphics_pipeline_create_info)
            .value
    };
}

auto create_command_pool(
    vk::Device t_device,
    uint32_t   t_queue_family_index
) noexcept -> engine::vulkan::CommandPool
{
    vk::CommandPoolCreateInfo command_pool_create_info{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = t_queue_family_index
    };
    return engine::vulkan::CommandPool(
        t_device, t_device.createCommandPool(command_pool_create_info).value
    );
}

[[nodiscard]] auto create_command_buffers(
    vk::Device      t_device,
    vk::CommandPool t_command_pool,
    uint32_t        t_count
) noexcept -> std::vector<vk::CommandBuffer>
{
    vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = t_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = t_count
    };
    return t_device.allocateCommandBuffers(command_buffer_allocate_info).value;
}

auto create_descriptor_pool(vk::Device t_device, uint32_t t_count) noexcept
    -> engine::vulkan::DescriptorPool
{
    vk::DescriptorPoolSize descriptor_pool_size{
        .type = vk::DescriptorType::eUniformBuffer, .descriptorCount = t_count
    };
    vk::DescriptorPoolCreateInfo descriptor_pool_create_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = t_count,
        .poolSizeCount = 1,
        .pPoolSizes    = &descriptor_pool_size
    };
    return engine::vulkan::DescriptorPool{
        t_device,
        t_device.createDescriptorPool(descriptor_pool_create_info).value
    };
}

auto create_semaphores(vk::Device t_device, uint32_t t_count) noexcept
    -> std::vector<engine::vulkan::Semaphore>
{
    vk::SemaphoreCreateInfo        createInfo{};
    std::vector<vulkan::Semaphore> semaphores;
    semaphores.reserve(t_count);

    for (uint32_t i = 0; i < t_count; i++) {
        if (!*semaphores.emplace_back(
                t_device, t_device.createSemaphore(createInfo).value
            ))
        {
            return {};
        }
    }

    return semaphores;
}

auto create_fences(vk::Device t_device, uint32_t t_count) noexcept
    -> std::vector<engine::vulkan::Fence>
{
    vk::FenceCreateInfo fence_create_info{
        .flags = vk::FenceCreateFlagBits::eSignaled
    };
    std::vector<vulkan::Fence> fences;
    fences.reserve(t_count);

    for (uint32_t i = 0; i < t_count; i++) {
        if (!*fences.emplace_back(
                t_device, t_device.createFence(fence_create_info).value
            ))
        {
            return {};
        }
    }

    return fences;
}

auto create_model(const std::string& t_path) noexcept
    -> tl::optional<gfx::Model>
{
    const auto format{ t_path.ends_with(".glb")
                           ? asset_manager::GltfFormat::eBinary
                           : asset_manager::GltfFormat::eAscii };

    return asset_manager::GltfLoader::load_model(format, t_path)
        .transform([]<typename Model>(Model&& t_model) {
            return gfx::ModelFactory::create_model(std::forward<Model>(t_model)
            );
        });
}

[[nodiscard]] static auto count_meshes(const gfx::Model::Node& t_node) noexcept
    -> uint32_t
{
    uint32_t count{};
    if (t_node.mesh) {
        count++;
    }
    for (const auto& child : t_node.children) {
        count += count_meshes(child);
    }
    return count;
}

auto count_meshes(const gfx::Model& t_model) noexcept -> uint32_t
{
    uint32_t count{};
    for (const auto& node : t_model.nodes()) {
        count += count_meshes(node);
    }
    return count;
}

}   // namespace init
