module;

#include <filesystem>
#include <functional>

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

module demos.gltf.DemoApp;

import ddge.modules.cache.Handle;
import ddge.modules.gltf.Model;
import ddge.modules.renderer.base.swapchain.Swapchain;
import ddge.modules.renderer.base.swapchain.SwapchainHolder;
import ddge.modules.renderer.scene.Scene;
import ddge.modules.window.Window;

import examples.base.init;

import demos.gltf.init;

auto demo::DemoPlugin::operator()(
    ddge::cache::Cache&                    cache,
    const ddge::renderer::base::Device&    device,
    const ddge::renderer::base::Allocator& allocator,
    ddge::renderer::base::SwapchainHolder& swapchain_holder
) const -> DemoApp
{
    return DemoApp{ cache,          device,
                    allocator,      swapchain_holder,
                    model_filepath, use_virtual_images };
}

template <std::invocable<vk::CommandBuffer> Executor>
static auto
    execute_command(const ddge::renderer::base::Device& device, Executor&& executor)
        -> void
{
    const vk::UniqueCommandPool command_pool{ examples::base::init::create_command_pool(
        device.get(), device.info().get_queue_index(vkb::QueueType::graphics).value()
    ) };
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = command_pool.get(),
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    const vk::CommandBuffer command_buffer{
        device->allocateCommandBuffers(command_buffer_allocate_info).front()
    };
    constexpr static vk::CommandBufferBeginInfo begin_info{};
    command_buffer.begin(begin_info);


    std::invoke(std::forward<Executor>(executor), command_buffer);


    command_buffer.end();
    const vk::SubmitInfo submit_info{
        .commandBufferCount = 1,
        .pCommandBuffers    = &command_buffer,
    };
    vk::UniqueFence fence{ device->createFenceUnique({}) };
    static_cast<vk::Queue>(device.info().get_queue(vkb::QueueType::graphics).value())
        .submit(submit_info, fence.get());
    std::ignore =
        device->waitForFences(std::array{ fence.get() }, vk::True, 100'000'000'000);
    device->resetCommandPool(command_pool.get());
}

[[nodiscard]]
static auto load_scene(
    const ddge::renderer::base::Device&    device,
    const ddge::renderer::base::Allocator& allocator,
    const vk::RenderPass                   render_pass,
    ddge::gltf::Model&&                    model,
    ddge::cache::Cache&                    cache,
    const bool                             use_virtual_images
) -> ddge::renderer::Scene
{
    auto packaged_scene{
        ddge::renderer::Scene::create()
            .add_model(ddge::cache::make_handle<const ddge::gltf::Model>(std::move(model)))
            .set_cache(cache)
            .build(device, allocator, render_pass, use_virtual_images)
    };

    ::execute_command(device, packaged_scene);

    return packaged_scene.get_future().get();
}

demo::DemoApp::DemoApp(
    ddge::cache::Cache&                    cache,
    const ddge::renderer::base::Device&    device,
    const ddge::renderer::base::Allocator& allocator,
    ddge::renderer::base::SwapchainHolder& swapchain_holder,
    const std::filesystem::path&           model_filepath,
    const bool                             use_virtual_images
)
    : m_allocator{ allocator },
      m_device{ device },
      m_render_pass{
          init::create_render_pass(swapchain_holder.get().value().format(), device)
      },
      m_depth_image{ init::create_depth_image(
          device.physical_device(),
          allocator,
          swapchain_holder.get().value().extent()
      ) },
      m_depth_image_view{ init::create_depth_image_view(device, m_depth_image.get()) },
      m_framebuffers{ init::create_framebuffers(
          device.get(),
          swapchain_holder.get().value().extent(),
          swapchain_holder.get().value().image_views(),
          m_render_pass.get(),
          m_depth_image_view.get()
      ) },
      m_scene{ ::load_scene(
          device,
          allocator,
          m_render_pass.get(),
          ddge::gltf::Loader::load_from_file(model_filepath).value(),
          cache,
          use_virtual_images
      ) }
{
    swapchain_holder.on_swapchain_recreated(
        [&](const ddge::renderer::base::Swapchain& swapchain) {
            m_depth_image.reset();
            m_depth_image = init::create_depth_image(
                device.physical_device(), allocator, swapchain.extent()
            );
        }
    );
    swapchain_holder.on_swapchain_recreated([&](const ddge::renderer::base::Swapchain&) {
        m_depth_image_view.reset();
        m_depth_image_view = init::create_depth_image_view(device, m_depth_image.get());
    });
    swapchain_holder.on_swapchain_recreated(
        [&](const ddge::renderer::base::Swapchain& swapchain) {
            m_framebuffers.clear();
            m_framebuffers = init::create_framebuffers(
                device.get(),
                swapchain.extent(),
                swapchain.image_views(),
                m_render_pass.get(),
                m_depth_image_view.get()
            );
        }
    );
}

auto demo::DemoApp::record_command_buffer(
    const uint32_t           image_index,
    const vk::Extent2D       swapchain_extent,
    const vk::CommandBuffer  graphics_command_buffer,
    const ddge::gfx::Camera& camera
) -> void
{
    constexpr static std::array clear_values{
        vk::ClearValue{
            .color = vk::ClearColorValue{ std::array{ 0.01f, 0.01f, 0.01f, 0.01f } } },
        vk::ClearValue{ .depthStencil = vk::ClearDepthStencilValue{ .depth = 1.f } }
    };
    const vk::RenderPassBeginInfo render_pass_begin_info{
        .renderPass      = m_render_pass.get(),
        .framebuffer     = m_framebuffers[image_index].get(),
        .renderArea      = vk::Rect2D{ .extent = swapchain_extent },
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues    = clear_values.data()
    };
    graphics_command_buffer.beginRenderPass(
        render_pass_begin_info, vk::SubpassContents::eInline
    );

    ::execute_command(
        m_device,
        std::bind_front(
            &ddge::renderer::Scene::update,
            std::ref(m_scene),
            std::cref(camera),
            m_allocator,
            m_device.get().info().get_queue(vkb::QueueType::graphics).value()
        )
    );
    m_scene.draw(graphics_command_buffer, camera);

    graphics_command_buffer.endRenderPass();
}
