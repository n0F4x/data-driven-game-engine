#include "DemoRenderer.hpp"

#include <core/graphics/model/ModelLoader.hpp>
#include <core/renderer/scene/Builder.hpp>
#include <core/window/Window.hpp>

#include "demo_init.hpp"

using namespace core;

constexpr static uint32_t g_frame_count{ 1 };

[[nodiscard]]
static auto load_scene(
    const renderer::Device&          t_device,
    const renderer::Allocator&       t_allocator,
    vk::RenderPass                   t_render_pass,
    cache::Handle<graphics::Model>&& t_model
) -> tl::optional<renderer::Scene>
{
    auto opt_vertex_shader_module{
        renderer::ShaderModule::create(t_device.get(), "shaders/model_test2.vert.spv")
    };
    if (!opt_vertex_shader_module.has_value()) {
        return tl::nullopt;
    }
    auto opt_fragment_shader_module{
        renderer::ShaderModule::create(t_device.get(), "shaders/model_test2.frag.spv")
    };
    if (!opt_fragment_shader_module.has_value()) {
        return tl::nullopt;
    }


    auto                                transfer_command_pool{ init::create_command_pool(
        t_device.get(), t_device.info().get_queue_index(vkb::QueueType::graphics).value()
    ) };
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = transfer_command_pool.get(),
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    auto command_buffer{
        t_device->allocateCommandBuffers(command_buffer_allocate_info).front()
    };

    constexpr vk::CommandBufferBeginInfo begin_info{};
    command_buffer.begin(begin_info);
    renderer::Scene scene{
        renderer::Scene::create()
            .add_model(
                std::move(t_model),
                renderer::Effect{
                                 renderer::Shader{ cache::make_handle<renderer::ShaderModule>(
                                          std::move(opt_vertex_shader_module.value())
                                      ),
                                      "main" },
                                 renderer::Shader{ cache::make_handle<renderer::ShaderModule>(
                                          std::move(opt_fragment_shader_module.value())
                                      ),
                                      "main" } }
            )
            .build(t_device.get(), t_allocator, command_buffer, t_render_pass)
    };
    command_buffer.end();

    const vk::SubmitInfo submit_info{
        .commandBufferCount = 1,
        .pCommandBuffers    = &command_buffer,
    };
    vk::UniqueFence fence{ t_device->createFenceUnique({}) };

    static_cast<void>(static_cast<vk::Queue>(
                          t_device.info().get_queue(vkb::QueueType::graphics).value()
    )
                          .submit(1, &submit_info, fence.get()));

    static_cast<void>(
        t_device->waitForFences(std::array{ fence.get() }, true, 100'000'000'000)
    );
    t_device->resetCommandPool(transfer_command_pool.get());

    return scene;
}

auto DemoRenderer::create(Store& t_store, const std::string& t_model_filepath)
    -> tl::optional<DemoRenderer>
{
    const auto& window{ t_store.at<window::Window>() };
    auto&       device{ t_store.at<renderer::Device>() };
    auto&       allocator{ t_store.at<renderer::Allocator>() };

    auto& swapchain{ t_store.at<renderer::Swapchain>() };
    int   width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);
    swapchain.set_framebuffer_size(vk::Extent2D{ static_cast<uint32_t>(width),
                                                 static_cast<uint32_t>(height) });
    if (!swapchain.get()) {
        return tl::nullopt;
    }

    auto render_pass{
        init::create_render_pass(swapchain.get()->surface_format(), device)
    };
    if (!render_pass) {
        return tl::nullopt;
    }

    auto depth_image{ init::create_depth_image(
        device.physical_device(), allocator.get(), swapchain.get()->extent()
    ) };
    if (!depth_image.get()) {
        return tl::nullopt;
    }

    auto depth_image_view{ init::create_depth_image_view(device, depth_image.get()) };
    if (!depth_image_view) {
        return tl::nullopt;
    }

    auto framebuffers{ init::create_framebuffers(
        device.get(),
        swapchain.get()->extent(),
        swapchain.get()->image_views(),
        render_pass.get(),
        depth_image_view.get()
    ) };
    if (framebuffers.empty()) {
        return tl::nullopt;
    }

    auto command_pool{ init::create_command_pool(
        device.get(), device.info().get_queue_index(vkb::QueueType::graphics).value()
    ) };
    if (!command_pool) {
        return tl::nullopt;
    }

    auto command_buffers{
        init::create_command_buffers(device.get(), command_pool.get(), g_frame_count)
    };
    if (command_buffers.empty()) {
        return tl::nullopt;
    }

    auto image_acquired_semaphores{ init::create_semaphores(device.get(), g_frame_count) };
    if (image_acquired_semaphores.empty()) {
        return tl::nullopt;
    }

    auto render_finished_semaphores{
        init::create_semaphores(device.get(), g_frame_count)
    };
    if (render_finished_semaphores.empty()) {
        return tl::nullopt;
    }

    auto in_flight_fences{ init::create_fences(device.get(), g_frame_count) };
    if (in_flight_fences.empty()) {
        return tl::nullopt;
    }

    auto opt_model{ graphics::ModelLoader{}.load_from_file(t_model_filepath) };
    if (!opt_model.has_value()) {
        return tl::nullopt;
    }

    auto opt_scene{
        load_scene(device, allocator, render_pass.get(), std::move(opt_model.value()))
    };
    if (!opt_scene.has_value()) {
        return tl::nullopt;
    }

    return DemoRenderer{
        .device                     = device,
        .allocator                  = allocator,
        .swapchain                  = swapchain,
        .render_pass                = std::move(render_pass),
        .depth_image                = std::move(depth_image),
        .depth_image_view           = std::move(depth_image_view),
        .framebuffers               = std::move(framebuffers),
        .command_pool               = std::move(command_pool),
        .command_buffers            = std::move(command_buffers),
        .image_acquired_semaphores  = std::move(image_acquired_semaphores),
        .render_finished_semaphores = std::move(render_finished_semaphores),
        .in_flight_fences           = std::move(in_flight_fences),
        .scene                      = std::move(opt_scene.value()),
    };
}

auto DemoRenderer::render(
    const vk::Extent2D            t_framebuffer_size,
    const core::graphics::Camera& t_camera
) noexcept -> void
{
    swapchain.set_framebuffer_size(t_framebuffer_size);

    while (device->waitForFences({ in_flight_fences[frame_index].get() }, true, UINT64_MAX)
           == vk::Result::eTimeout)
        ;

    swapchain.acquire_next_image(image_acquired_semaphores[frame_index].get(), {})
        .transform([&](const uint32_t image_index) {
            device->resetFences({ in_flight_fences[frame_index].get() });
            command_buffers[frame_index].reset();

            record_command_buffer(image_index, t_camera);

            std::array wait_semaphores{ image_acquired_semaphores[frame_index].get() };
            std::array<vk::PipelineStageFlags, wait_semaphores.size()> wait_stages{
                vk::PipelineStageFlagBits::eColorAttachmentOutput
            };
            std::array signal_semaphores{ render_finished_semaphores[frame_index].get() };
            const vk::SubmitInfo submit_info{
                .waitSemaphoreCount   = static_cast<uint32_t>(wait_semaphores.size()),
                .pWaitSemaphores      = wait_semaphores.data(),
                .pWaitDstStageMask    = wait_stages.data(),
                .commandBufferCount   = 1,
                .pCommandBuffers      = &command_buffers[frame_index],
                .signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size()),
                .pSignalSemaphores    = signal_semaphores.data()
            };
            vk::Queue(device.info().get_queue(vkb::QueueType::graphics).value())
                .submit(submit_info, in_flight_fences[frame_index].get());

            swapchain.present(signal_semaphores);
        });

    frame_index = (frame_index + 1) % g_frame_count;
}

auto DemoRenderer::record_command_buffer(
    const uint32_t         t_image_index,
    core::graphics::Camera t_camera
) noexcept -> void
{
    const auto                           command_buffer = command_buffers[frame_index];
    constexpr vk::CommandBufferBeginInfo command_buffer_begin_info{};

    static_cast<void>(command_buffer.begin(command_buffer_begin_info));


    std::array clearValues{ vk::ClearValue{
                                .color = { std::array{ 0.01f, 0.01f, 0.01f, 0.01f } } },
                            vk::ClearValue{ .depthStencil = { 1.f, 0 } } };

    const auto extent{ swapchain.get()->extent() };
    command_buffer.setViewport(
        0,
        vk::Viewport{ .width    = static_cast<float>(extent.width),
                      .height   = static_cast<float>(extent.height),
                      .maxDepth = 1.f }
    );
    command_buffer.setScissor(0, vk::Rect2D{ {}, extent });

    const vk::RenderPassBeginInfo render_pass_begin_info{
        .renderPass      = render_pass.get(),
        .framebuffer     = framebuffers[t_image_index].get(),
        .renderArea      = { .extent = swapchain.get()->extent() },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues    = clearValues.data()
    };
    command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

    t_camera.set_perspective_projection(
        50.f,
        static_cast<float>(extent.width) / static_cast<float>(extent.height),
        0.1f,
        10000.f
    );

    scene.draw(command_buffer, t_camera);


    command_buffer.endRenderPass();
    command_buffer.end();
}