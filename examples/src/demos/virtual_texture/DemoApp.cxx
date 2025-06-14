module;

#include <ranges>

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

#include <glm/ext/matrix_float4x4.hpp>

module demos.virtual_texture.DemoApp;

import core.gfx.Camera;
import core.gfx.resources.image_helpers;
import core.renderer.base.device.Device;
import core.renderer.base.allocator.Allocator;
import core.renderer.base.resources.copy_operations;
import core.renderer.base.resources.Image;
import core.renderer.base.swapchain.Swapchain;
import core.renderer.base.swapchain.SwapchainHolder;

import extensions.renderer.DevicePlugin;
import extensions.renderer.InstancePlugin;

import demos.virtual_texture.init;
import demos.virtual_texture.Camera;

auto demo::DemoPlugin::setup(
    extensions::renderer::InstancePlugin& instance_plugin,
    extensions::renderer::DevicePlugin&   device_plugin
) -> void
{
    instance_plugin.emplace_dependency(
        extensions::renderer::InstancePlugin::Dependency{
            .required_settings_are_available = [](const vkb::SystemInfo& system_info
                                               ) -> bool {
                return system_info.is_extension_available(
                    vk::KHRGetPhysicalDeviceProperties2ExtensionName
                );
            },
            .enable_settings = [](const vkb::SystemInfo&,
                                  vkb::InstanceBuilder& instance_builder) -> void {
                instance_builder
                    .enable_extension(vk::KHRGetPhysicalDeviceProperties2ExtensionName)
                    .require_api_version(1, 2);
            },
        }
    );

    device_plugin.emplace_dependency(
        extensions::renderer::DevicePlugin::Dependency{
            .require_settings =
                [](vkb::PhysicalDeviceSelector& physical_device_selector) {
                    physical_device_selector.add_required_extension(
                        vk::KHRMaintenance2ExtensionName
                    );
                    physical_device_selector.add_required_extension(
                        vk::KHRMultiviewExtensionName
                    );
                    physical_device_selector.add_required_extension(
                        vk::KHRCreateRenderpass2ExtensionName
                    );
                    physical_device_selector.add_required_extension(
                        vk::KHRDepthStencilResolveExtensionName
                    );
                    physical_device_selector.add_required_extension(
                        vk::KHRDynamicRenderingExtensionName
                    );

                    constexpr static vk::PhysicalDeviceDynamicRenderingFeatures
                        dynamic_rendering_features{
                            .dynamicRendering = vk::True,
                        };
                    physical_device_selector.add_required_extension_features(
                        dynamic_rendering_features
                    );
                } }
    );
}

auto demo::DemoPlugin::operator()(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator,
    core::renderer::base::SwapchainHolder& swapchain_holder
) const -> DemoApp
{
    return DemoApp{ device, allocator, swapchain_holder };
}

demo::DemoApp::DemoApp(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator,
    core::renderer::base::SwapchainHolder& swapchain_holder
)
    : m_swapchain_holder_ref{ swapchain_holder },
      m_descriptor_set_layout{ demo::init::create_descriptor_set_layout(device.get()) },
      m_pipeline_layout{ demo::init::create_pipeline_layout(
          device.get(),
          std::array{ m_descriptor_set_layout.get() }
      ) },
      m_depth_image{ init::create_depth_image(
          device.physical_device(),
          allocator,
          swapchain_holder.get().value().extent()
      ) },
      m_depth_image_view{ init::create_depth_image_view(device, m_depth_image.get()) },
      m_descriptor_pool{ core::renderer::base::DescriptorPool::create()
                             .request_descriptor_sets(1)
                             .request_descriptors(
                                 vk::DescriptorPoolSize{
                                     .type = vk::DescriptorType::eUniformBuffer,
                                     .descriptorCount = 1,
                                 }
                             )
                             .request_descriptors(
                                 vk::DescriptorPoolSize{
                                     .type = vk::DescriptorType::eCombinedImageSampler,
                                     .descriptorCount = 1,
                                 }
                             )
                             .request_descriptors(
                                 vk::DescriptorPoolSize{
                                     .type = vk::DescriptorType::eUniformBuffer,
                                     .descriptorCount = 1,
                                 }
                             )
                             .request_descriptors(
                                 vk::DescriptorPoolSize{
                                     .type = vk::DescriptorType::eUniformBuffer,
                                     .descriptorCount = 1,
                                 }
                             )
                             // DEBUG
                             .request_descriptor_sets(1)
                             .request_descriptors(
                                 vk::DescriptorPoolSize{
                                     .type = vk::DescriptorType::eUniformBuffer,
                                     .descriptorCount = 1,
                                 }
                             )
                             .request_descriptors(
                                 vk::DescriptorPoolSize{
                                     .type = vk::DescriptorType::eCombinedImageSampler,
                                     .descriptorCount = 1,
                                 }
                             )
                             .build(device.get()) },
      m_debug_texture_pipeline{ init::create_pipeline(
          device.get(),
          m_pipeline_layout.get(),
          swapchain_holder.get().value().format(),
          m_depth_image.format(),
          "simple_image.frag.spv"
      ) },
      m_virtual_texture_pipeline{ init::create_pipeline(
          device.get(),
          m_pipeline_layout.get(),
          swapchain_holder.get().value().format(),
          m_depth_image.format(),
          "virtual_image_with_request.frag.spv"
      ) },
      m_camera_buffer{ init::create_camera_buffer(allocator) },
      m_virtual_texture{ device, allocator },
      m_virtual_texture_info_buffer{
          init::create_virtual_texture_info_buffer(allocator, m_virtual_texture.get())
      },
      m_virtual_texture_blocks_buffer{
          init::create_virtual_texture_blocks_buffer(allocator, m_virtual_texture.get())
      },
      m_virtual_texture_blocks_uniform{ init::create_virtual_texture_blocks_uniform(
          device.get(),
          allocator,
          m_virtual_texture_blocks_buffer.buffer().get()
      ) },
      m_virtual_texture_descriptor_set{ init::create_virtual_texture_descriptor_set(
          device.get(),
          m_descriptor_set_layout.get(),
          m_descriptor_pool.get(),
          m_camera_buffer.buffer(),
          m_virtual_texture,
          m_virtual_texture_info_buffer.buffer(),
          m_virtual_texture_blocks_uniform.buffer()
      ) },
      m_debug_texture_descriptor_set{ init::create_debug_texture_descriptor_set(
          device.get(),
          m_descriptor_set_layout.get(),
          m_descriptor_pool.get(),
          m_camera_buffer.buffer(),
          m_virtual_texture.debug_view(),
          m_virtual_texture.debug_sampler()
      ) }
{
    swapchain_holder.on_swapchain_recreated(
        [&](const core::renderer::base::Swapchain& swapchain) {
            m_depth_image.reset();
            m_depth_image = init::create_depth_image(
                device.physical_device(), allocator, swapchain.extent()
            );
        }
    );
}

static auto transition_image_layout(
    const vk::CommandBuffer          command_buffer,
    const vk::Image                  image,
    const vk::PipelineStageFlags     src_stage_mask,
    const vk::PipelineStageFlags     dst_stage_mask,
    const vk::AccessFlags            src_access_mask,
    const vk::AccessFlags            dst_access_mask,
    const vk::ImageLayout            old_layout,
    const vk::ImageLayout            new_layout,
    const vk::ImageSubresourceRange& subresource_range
) -> void
{
    // Create an image barrier object
    const vk::ImageMemoryBarrier image_memory_barrier{
        .srcAccessMask       = src_access_mask,
        .dstAccessMask       = dst_access_mask,
        .oldLayout           = old_layout,
        .newLayout           = new_layout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image               = image,
        .subresourceRange    = subresource_range,
    };

    command_buffer.pipelineBarrier(
        src_stage_mask, dst_stage_mask, {}, {}, {}, image_memory_barrier
    );
}

auto demo::DemoApp::render(
    const uint32_t           image_index,
    const vk::Extent2D       swapchain_extent,
    const vk::CommandBuffer  graphics_command_buffer,
    const core::gfx::Camera& camera
) -> void
{
    constexpr static vk::ImageSubresourceRange color_image_subresource_range{
        .aspectMask     = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel   = 0,
        .levelCount     = vk::RemainingMipLevels,
        .baseArrayLayer = 0,
        .layerCount     = vk::RemainingArrayLayers,
    };

    ::transition_image_layout(
        graphics_command_buffer,
        m_swapchain_holder_ref.get().get().value().images().at(image_index),
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {},
        vk::AccessFlagBits::eColorAttachmentWrite,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        color_image_subresource_range
    );

    ::core::gfx::resources::transition_image_layout(
        graphics_command_buffer,
        m_depth_image,
        core::renderer::base::Image::State{
            .stage_mask = vk::PipelineStageFlagBits::eEarlyFragmentTests
                        | vk::PipelineStageFlagBits::eLateFragmentTests,
            .access_mask = vk::AccessFlagBits::eDepthStencilAttachmentRead
                         | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
            .layout = vk::ImageLayout::eDepthAttachmentOptimal }
    );

    constexpr static std::array clear_values{
        vk::ClearValue{
            .color = vk::ClearColorValue{ std::array{ 0.01f, 0.01f, 0.01f, 0.01f } } },
        vk::ClearValue{ .depthStencil = vk::ClearDepthStencilValue{ .depth = 1.f } }
    };

    const vk::RenderingAttachmentInfoKHR color_attachment_info{
        .imageView =
            m_swapchain_holder_ref.get().get().value().image_views().at(image_index).get(),
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .resolveMode = vk::ResolveModeFlagBits::eNone,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eStore,
        .clearValue  = clear_values[0],
    };

    const vk::RenderingAttachmentInfoKHR depth_attachment_info{
        .imageView   = m_depth_image_view.get(),
        .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .resolveMode = vk::ResolveModeFlagBits::eNone,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eDontCare,
        .clearValue  = clear_values[1],
    };

    const vk::Rect2D render_area{
        .offset = vk::Offset2D{},
        .extent = swapchain_extent,
    };
    const vk::RenderingInfoKHR render_info{
        .renderArea           = render_area,
        .layerCount           = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &color_attachment_info,
        .pDepthAttachment     = &depth_attachment_info,
    };

    graphics_command_buffer.beginRendering(render_info);

    draw(graphics_command_buffer, camera);

    graphics_command_buffer.endRendering();

    ::transition_image_layout(
        graphics_command_buffer,
        m_swapchain_holder_ref.get().get().value().images().at(image_index),
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eBottomOfPipe,
        vk::AccessFlagBits::eColorAttachmentWrite,
        {},
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        color_image_subresource_range
    );
}

auto demo::DemoApp::draw(
    const vk::CommandBuffer  graphics_command_buffer,
    const core::gfx::Camera& camera
) -> void
{
    m_camera_buffer.set(
        Camera{
            .position   = glm::vec4{ camera.position(), 1 },
            .view       = camera.view(),
            .projection = camera.projection(),
    }
    );

    graphics_command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_pipeline_layout.get(),
        0,
        m_virtual_texture_descriptor_set.get(),
        {}
    );

    graphics_command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, m_virtual_texture_pipeline.get()
    );

    m_virtual_texture.draw(graphics_command_buffer);
    update_virtual_texture(camera);

    draw_debug(graphics_command_buffer);
}

auto demo::DemoApp::update_virtual_texture([[maybe_unused]] const core::gfx::Camera& camera
) -> void
{
    // Update based on feedback from shader
    std::vector<uint32_t> virtual_texture_blocks(m_virtual_texture.get().blocks().size());
    core::renderer::base::copy(
        core::renderer::base::CopyRegion{
            .allocation = m_virtual_texture_blocks_buffer.allocation() },
        virtual_texture_blocks.data(),
        virtual_texture_blocks.size() * sizeof(uint32_t)
    );
    for (const uint32_t block_index :
         std::views::iota(0u, m_virtual_texture.get().blocks().size()))
    {
        if (virtual_texture_blocks.at(block_index) > 0) {
            m_virtual_texture.get().request_block(block_index);
            virtual_texture_blocks.at(block_index) = 0;
        }
    }
    core::renderer::base::copy(
        virtual_texture_blocks.data(),
        core::renderer::base::CopyRegion{
            .allocation = m_virtual_texture_blocks_buffer.allocation(),
        },
        virtual_texture_blocks.size() * sizeof(uint32_t)
    );

    // // Update based on distance from texture to camera
    // m_virtual_texture.get().request_blocks_by_distance_from_camera(
    //     glm::distance(camera.position(), m_virtual_texture.position()), 10
    // );
}

auto demo::DemoApp::draw_debug(const vk::CommandBuffer graphics_command_buffer) -> void
{
    graphics_command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        m_pipeline_layout.get(),
        0,
        m_debug_texture_descriptor_set.get(),
        {}
    );

    graphics_command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics, m_debug_texture_pipeline.get()
    );

    m_virtual_texture.draw_debug(graphics_command_buffer);
}
