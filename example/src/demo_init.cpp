#include "demo_init.hpp"

#include <print>

#include <vulkan/vulkan.hpp>

#include <core/renderer/base/Device.hpp>
#include <core/renderer/model/Model.hpp>
#include <core/utility/vulkan/tools.hpp>

using namespace core;

namespace {

[[nodiscard]] auto find_supported_format(
    const vk::PhysicalDevice       t_physical_device,
    const std::vector<vk::Format>& t_candidates,
    const vk::ImageTiling          t_tiling,
    const vk::FormatFeatureFlags   t_features
) -> vk::Format
{
    for (const auto format : t_candidates) {
        vk::FormatProperties format_properties;
        t_physical_device.getFormatProperties(format, &format_properties);

        if ((t_tiling == vk::ImageTiling::eLinear
             && (format_properties.linearTilingFeatures & t_features) == t_features)
            || (t_tiling == vk::ImageTiling::eOptimal
                && (format_properties.optimalTilingFeatures & t_features) == t_features))
        {
            return format;
        }
    }
    throw std::runtime_error{ "Failed to find supported format!" };
}

[[nodiscard]] auto find_depth_format(const vk::PhysicalDevice t_physical_device) noexcept
    -> vk::Format
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

auto create_render_pass(
    const vk::SurfaceFormatKHR& t_surface_format,
    const renderer::Device&     t_device
) -> vk::UniqueRenderPass
{
    const vk::AttachmentDescription color_attachment_description{
        .format         = t_surface_format.format,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    };
    vk::AttachmentReference color_attachment_reference{
        .attachment = 0,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    };

    const vk::AttachmentDescription depth_attachment_description{
        .format         = find_depth_format(t_device.physical_device()),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
    vk::AttachmentReference depth_attachment_reference{
        .attachment = 1,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    std::array attachment_descriptions{ color_attachment_description,
                                        depth_attachment_description };

    vk::SubpassDescription subpass_description{
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &color_attachment_reference,
        .pDepthStencilAttachment = &depth_attachment_reference,
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

    const vk::RenderPassCreateInfo render_pass_create_info{
        .attachmentCount = static_cast<uint32_t>(attachment_descriptions.size()),
        .pAttachments    = attachment_descriptions.data(),
        .subpassCount    = 1,
        .pSubpasses      = &subpass_description,
        .dependencyCount = 1,
        .pDependencies   = &subpass_dependency,
    };

    return t_device->createRenderPassUnique(render_pass_create_info);
}

auto create_depth_image(
    const vk::PhysicalDevice t_physical_device,
    VmaAllocator             t_allocator,
    const vk::Extent2D       t_swapchain_extent
) noexcept -> vma::Image
{
    const vk::ImageCreateInfo image_create_info = {
        .imageType = vk::ImageType::e2D,
        .format    = find_depth_format(t_physical_device),
        .extent = vk::Extent3D{ t_swapchain_extent.width, t_swapchain_extent.height, 1 },
        .mipLevels     = 1,
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = vk::ImageTiling::eOptimal,
        .usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    constexpr VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    vk::Image     image{};
    VmaAllocation allocation;
    vmaCreateImage(
        t_allocator,
        reinterpret_cast<const VkImageCreateInfo*>(&image_create_info),
        &allocation_create_info,
        reinterpret_cast<VkImage*>(&image),
        &allocation,
        nullptr
    );
    return vma::Image(t_allocator, image, allocation);
}

auto create_depth_image_view(
    const renderer::Device& t_device,
    const vk::Image         t_depth_image
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo image_view_create_info{
        .image    = t_depth_image,
        .viewType = vk::ImageViewType::e2D,
        .format   = find_depth_format(t_device.physical_device()),
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };

    return t_device->createImageViewUnique(image_view_create_info);
}

auto create_framebuffers(
    const vk::Device                        t_device,
    const vk::Extent2D                      t_swapchain_extent,
    const std::vector<vk::UniqueImageView>& t_swapchain_image_views,
    const vk::RenderPass                    t_render_pass,
    const vk::ImageView                     t_depth_image_view
) -> std::vector<vk::UniqueFramebuffer>
{
    std::vector<vk::UniqueFramebuffer> framebuffers;
    framebuffers.reserve(t_swapchain_image_views.size());

    for (const auto& swapchain_image_view : t_swapchain_image_views) {
        std::array attachments{ *swapchain_image_view, t_depth_image_view };

        vk::FramebufferCreateInfo framebuffer_create_info{
            .renderPass      = t_render_pass,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments    = attachments.data(),
            .width           = t_swapchain_extent.width,
            .height          = t_swapchain_extent.height,
            .layers          = 1
        };

        framebuffers.emplace_back(t_device.createFramebufferUnique(framebuffer_create_info
        ));
    }

    return framebuffers;
}

auto create_descriptor_set_layout(const vk::Device t_device)
    -> vk::UniqueDescriptorSetLayout
{
    constexpr static std::array descriptor_set_layout_bindings{
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 0,
                                       .descriptorType  = vk::DescriptorType::eUniformBuffer,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eVertex  },
        vk::DescriptorSetLayoutBinding{
                                       .binding         = 1,
                                       .descriptorType  = vk::DescriptorType::eCombinedImageSampler,
                                       .descriptorCount = 1,
                                       .stageFlags      = vk::ShaderStageFlagBits::eFragment }
    };

    constexpr static vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{
        .bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings.size()),
        .pBindings    = descriptor_set_layout_bindings.data()
    };

    return t_device.createDescriptorSetLayoutUnique(descriptor_set_layout_create_info);
}

auto create_pipeline_layout(
    const vk::Device              t_device,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const uint32_t                t_push_constant_size
) -> vk::UniquePipelineLayout
{
    const vk::PushConstantRange push_constant_range{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .size       = t_push_constant_size,
    };
    const vk::PipelineLayoutCreateInfo pipeline_layout_create_info{
        .setLayoutCount         = 1,
        .pSetLayouts            = &t_descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &push_constant_range,
    };

    return t_device.createPipelineLayoutUnique(pipeline_layout_create_info);
}

auto create_pipeline(
    const vk::Device         t_device,
    const vk::PipelineLayout t_pipeline_layout,
    const vk::RenderPass     t_render_pass
) -> vk::UniquePipeline
{
    const vk::UniqueShaderModule vertex_shader_module{
        vulkan::load_shader(t_device, "shaders/model_test.vert.spv")
    };
    const vk::UniqueShaderModule fragment_shader_module{
        vulkan::load_shader(t_device, "shaders/model_test.frag.spv")
    };
    if (!vertex_shader_module || !fragment_shader_module) {
        std::println("Failed loading shaders");
        throw std::runtime_error{ "Failed loading shaders!" };
    }

    std::array pipeline_shader_stage_create_infos{
        vk::PipelineShaderStageCreateInfo{   .stage  = vk::ShaderStageFlagBits::eVertex,
                                          .module = *vertex_shader_module,
                                          .pName  = "main" },
        vk::PipelineShaderStageCreateInfo{ .stage  = vk::ShaderStageFlagBits::eFragment,
                                          .module = *fragment_shader_module,
                                          .pName  = "main" }
    };

    using Vertex = renderer::Model::Vertex;
    vk::VertexInputBindingDescription vertex_input_binding_description{
        .binding = 0, .stride = sizeof(Vertex), .inputRate = vk::VertexInputRate::eVertex
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
                                            .offset   = static_cast<uint32_t>(offsetof(Vertex,   normal)) },
        vk::VertexInputAttributeDescription{
                                            .location = 2,
                                            .binding  = 0,
                                            .format   = vk::Format::eR32G32Sfloat,
                                            .offset   = static_cast<uint32_t>(offsetof(Vertex,     uv_0)) },
        vk::VertexInputAttributeDescription{
                                            .location = 3,
                                            .binding  = 0,
                                            .format   = vk::Format::eR32G32B32A32Sfloat,
                                            .offset   = static_cast<uint32_t>(offsetof(Vertex,    color)) }
    };

    vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info{
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions    = &vertex_input_binding_description,
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertex_input_attribute_descriptions.size()),
        .pVertexAttributeDescriptions = vertex_input_attribute_descriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{
        .topology = vk::PrimitiveTopology::eTriangleList
    };

    vk::PipelineViewportStateCreateInfo pipeline_viewport_state_create_info{
        .viewportCount = 1, .scissorCount = 1
    };

    vk::PipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info{
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode    = vk::CullModeFlagBits::eBack,
        .frontFace   = vk::FrontFace::eCounterClockwise,
        .lineWidth   = 1.f
    };

    vk::PipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{
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
        .blendEnable    = false,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                        | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };
    vk::PipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info{
        .attachmentCount = 1,
        .pAttachments    = &pipeline_color_blend_attachment_state,
    };

    std::array dynamic_states{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates    = dynamic_states.data()
    };

    vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info{
        .stageCount = static_cast<uint32_t>(pipeline_shader_stage_create_infos.size()),
        .pStages    = pipeline_shader_stage_create_infos.data(),
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

    return t_device.createGraphicsPipelineUnique(nullptr, graphics_pipeline_create_info)
        .value;
}

auto create_command_pool(const vk::Device t_device, const uint32_t t_queue_family_index)
    -> vk::UniqueCommandPool
{
    const vk::CommandPoolCreateInfo command_pool_create_info{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = t_queue_family_index
    };

    return t_device.createCommandPoolUnique(command_pool_create_info);
}

auto create_command_buffers(
    const vk::Device      t_device,
    const vk::CommandPool t_command_pool,
    const uint32_t        t_count
) -> std::vector<vk::CommandBuffer>
{
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = t_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = t_count
    };

    return t_device.allocateCommandBuffers(command_buffer_allocate_info);
}

auto create_descriptor_pool(const vk::Device t_device, const uint32_t t_count)
    -> vk::UniqueDescriptorPool
{
    const std::array descriptor_pool_sizes{
        vk::DescriptorPoolSize{ .type            = vk::DescriptorType::eUniformBuffer,
                               .descriptorCount = t_count },
        vk::DescriptorPoolSize{     .type = vk::DescriptorType::eCombinedImageSampler,
                               .descriptorCount = 1      },
    };
    const vk::DescriptorPoolCreateInfo descriptor_pool_create_info{
        .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets       = t_count,
        .poolSizeCount = static_cast<uint32_t>(descriptor_pool_sizes.size()),
        .pPoolSizes    = descriptor_pool_sizes.data()
    };

    return t_device.createDescriptorPoolUnique(descriptor_pool_create_info);
}

auto create_semaphores(const vk::Device t_device, const uint32_t t_count)
    -> std::vector<vk::UniqueSemaphore>
{
    constexpr vk::SemaphoreCreateInfo createInfo{};
    std::vector<vk::UniqueSemaphore>  semaphores;
    semaphores.reserve(t_count);

    for (uint32_t i = 0; i < t_count; i++) {
        semaphores.emplace_back(t_device.createSemaphoreUnique(createInfo));
    }

    return semaphores;
}

auto create_fences(const vk::Device t_device, const uint32_t t_count)
    -> std::vector<vk::UniqueFence>
{
    constexpr vk::FenceCreateInfo fence_create_info{
        .flags = vk::FenceCreateFlagBits::eSignaled
    };
    std::vector<vk::UniqueFence> fences;
    fences.reserve(t_count);

    for (uint32_t i = 0; i < t_count; i++) {
        fences.emplace_back(t_device.createFenceUnique(fence_create_info));
    }

    return fences;
}

[[nodiscard]] static auto count_meshes(const renderer::StagingModel::Node& t_node
) noexcept -> uint32_t
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

auto count_meshes(const renderer::StagingModel& t_model) noexcept -> uint32_t
{
    uint32_t count{};

    for (const auto& node : t_model.nodes()) {
        count += count_meshes(node);
    }

    return count;
}

auto upload_model(
    const renderer::Device&       t_device,
    const renderer::Allocator&    t_allocator,
    renderer::StagingModel&&      t_staging_model,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool
) -> tl::optional<renderer::RenderModel>
{
    auto transfer_command_pool{
        create_command_pool(*t_device, t_device.transfer_queue_family_index())
    };
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = *transfer_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    auto command_buffer{
        t_device->allocateCommandBuffers(command_buffer_allocate_info).front()
    };

    constexpr vk::CommandBufferBeginInfo begin_info{};
    command_buffer.begin(begin_info);
    auto opt_model{ std::move(t_staging_model)
                        .upload(
                            *t_device,
                            t_allocator,
                            command_buffer,
                            t_descriptor_set_layout,
                            t_descriptor_pool
                        ) };
    command_buffer.end();

    const vk::SubmitInfo submit_info{
        .commandBufferCount = 1,
        .pCommandBuffers    = &command_buffer,
    };
    vk::UniqueFence fence{ t_device->createFenceUnique({}) };

    static_cast<void>(t_device.transfer_queue().submit(1, &submit_info, *fence));

    const auto raw_fence{ *fence };
    static_cast<void>(t_device->waitForFences(1, &raw_fence, true, 100'000'000'000));
    t_device->resetCommandPool(*transfer_command_pool);

    return opt_model;
}

}   // namespace init
