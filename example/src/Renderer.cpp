#include "Renderer.hpp"

#include <core/gltf/Loader.hpp>
#include <core/renderer/scene/Builder.hpp>
#include <core/window/Window.hpp>

#include "init.hpp"

constexpr static uint32_t frame_count_v{ 1 };

[[nodiscard]]
static auto load_scene(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator,
    const vk::RenderPass                   render_pass,
    core::gltf::Model&&                    model,
    core::cache::Cache&                    cache
) -> std::optional<core::renderer::Scene>
{
    auto                                transfer_command_pool{ init::create_command_pool(
        device.get(), device.info().get_queue_index(vkb::QueueType::graphics).value()
    ) };
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = transfer_command_pool.get(),
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    auto command_buffer{
        device->allocateCommandBuffers(command_buffer_allocate_info).front()
    };

    auto packaged_scene{
        core::renderer::Scene::create()
            .add_model(core::cache::make_handle<const core::gltf::Model>(std::move(model)))
            .set_cache(cache)
            .build(device, allocator, render_pass)
    };

    constexpr vk::CommandBufferBeginInfo begin_info{};
    command_buffer.begin(begin_info);
    std::invoke(packaged_scene, command_buffer);
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
    device->resetCommandPool(transfer_command_pool.get());

    return packaged_scene.get_future().get();
}

auto DemoRenderer::create(Store& store, const std::filesystem::path& model_filepath)
    -> std::optional<DemoRenderer>
{
    auto&       cache{ store.at<core::cache::Cache>() };
    const auto& window{ store.at<core::window::Window>() };
    const auto& device{ store.at<core::renderer::base::Device>() };
    const auto& allocator{ store.at<core::renderer::base::Allocator>() };

    auto& swapchain{ store.at<core::renderer::base::SwapchainHolder>() };
    swapchain.set_framebuffer_size(static_cast<vk::Extent2D>(window.framebuffer_size()));
    if (!swapchain.get().has_value()) {
        return std::nullopt;
    }
    const auto& raw_swapchain{ swapchain.get().value() };

    auto render_pass{ init::create_render_pass(raw_swapchain.format(), device) };
    if (!render_pass) {
        return std::nullopt;
    }

    auto depth_image{ init::create_depth_image(
        device.physical_device(), allocator, raw_swapchain.extent()
    ) };
    if (!depth_image.get()) {
        return std::nullopt;
    }

    auto depth_image_view{ init::create_depth_image_view(device, depth_image.get()) };
    if (!depth_image_view) {
        return std::nullopt;
    }

    auto framebuffers{ init::create_framebuffers(
        device.get(),
        raw_swapchain.extent(),
        raw_swapchain.image_views(),
        render_pass.get(),
        depth_image_view.get()
    ) };
    if (framebuffers.empty()) {
        return std::nullopt;
    }

    auto command_pool{ init::create_command_pool(
        device.get(), device.info().get_queue_index(vkb::QueueType::graphics).value()
    ) };
    if (!command_pool) {
        return std::nullopt;
    }

    auto command_buffers{
        init::create_command_buffers(device.get(), command_pool.get(), frame_count_v)
    };
    if (command_buffers.empty()) {
        return std::nullopt;
    }

    auto image_acquired_semaphores{ init::create_semaphores(device.get(), frame_count_v) };
    if (image_acquired_semaphores.empty()) {
        return std::nullopt;
    }

    auto render_finished_semaphores{
        init::create_semaphores(device.get(), frame_count_v)
    };
    if (render_finished_semaphores.empty()) {
        return std::nullopt;
    }

    auto in_flight_fences{ init::create_fences(device.get(), frame_count_v) };
    if (in_flight_fences.empty()) {
        return std::nullopt;
    }

    auto opt_model{ core::gltf::Loader::load_from_file(model_filepath) };
    if (!opt_model.has_value()) {
        return std::nullopt;
    }

    auto opt_scene{ load_scene(
        device, allocator, render_pass.get(), std::move(opt_model.value()), cache
    ) };
    if (!opt_scene.has_value()) {
        return std::nullopt;
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
        .render_finished_fences     = std::move(in_flight_fences),
        .scene                      = std::move(opt_scene.value()),
    };
}

static auto submit_render(
    const vk::Device        device,
    const vk::Queue         queue,
    const vk::Semaphore     wait_semaphore,
    const vk::CommandBuffer command_buffer,
    const vk::Semaphore     signal_semaphore,
    const vk::Fence         signal_fence
) -> void
{
    constexpr vk::PipelineStageFlags wait_pipeline_stage{
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    const vk::SubmitInfo submit_info{
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &wait_semaphore,
        .pWaitDstStageMask    = &wait_pipeline_stage,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &signal_semaphore,
    };

    device.resetFences(signal_fence);
    queue.submit(submit_info, signal_fence);
}

auto DemoRenderer::render(
    const vk::Extent2D            framebuffer_size,
    const core::gfx::Camera& camera
) -> void
{
    swapchain.get().set_framebuffer_size(framebuffer_size);
    if (!swapchain.get().get().has_value()) {
        return;
    }

    std::ignore = device.get()->waitForFences(
        render_finished_fences[frame_index].get(),
        vk::True,
        std::numeric_limits<uint64_t>::max()
    );

    if (const std::optional<uint32_t> image_index{ swapchain.get().acquire_next_image(
            image_acquired_semaphores[frame_index].get()
        ) };
        image_index.has_value())
    {
        command_buffers[frame_index].reset();
        record_command_buffer(
            swapchain.get().get().value().extent(), image_index.value(), camera
        );

        submit_render(
            device.get().get(),
            device.get().info().get_queue(vkb::QueueType::graphics).value(),
            image_acquired_semaphores[frame_index].get(),
            command_buffers[frame_index],
            render_finished_semaphores[frame_index].get(),
            render_finished_fences[frame_index].get()
        );

        swapchain.get().present(render_finished_semaphores[frame_index].get());
    }

    frame_index = (frame_index + 1) % frame_count_v;
}

auto DemoRenderer::record_command_buffer(
    const vk::Extent2D     swapchain_extent,
    const uint32_t         image_index,
    core::gfx::Camera camera
) -> void
{
    const vk::CommandBuffer command_buffer{ command_buffers[frame_index] };
    constexpr static vk::CommandBufferBeginInfo command_buffer_begin_info{};

    command_buffer.begin(command_buffer_begin_info);


    constexpr std::array clear_values{
        vk::ClearValue{
            .color = vk::ClearColorValue{ std::array{ 0.01f, 0.01f, 0.01f, 0.01f } } },
        vk::ClearValue{ .depthStencil = vk::ClearDepthStencilValue{ .depth = 1.f } }
    };

    command_buffer.setViewport(
        0,
        vk::Viewport{ .width    = static_cast<float>(swapchain_extent.width),
                      .height   = static_cast<float>(swapchain_extent.height),
                      .maxDepth = 1.f }
    );
    command_buffer.setScissor(0, vk::Rect2D{ .extent = swapchain_extent });

    const vk::RenderPassBeginInfo render_pass_begin_info{
        .renderPass      = render_pass.get(),
        .framebuffer     = framebuffers[image_index].get(),
        .renderArea      = vk::Rect2D{ .extent = swapchain_extent },
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues    = clear_values.data()
    };
    command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

    camera.set_perspective_projection(
        50.f,
        static_cast<float>(swapchain_extent.width)
            / static_cast<float>(swapchain_extent.height),
        0.01f,
        10000.f
    );

    scene.draw(command_buffer, camera);


    command_buffer.endRenderPass();
    command_buffer.end();
}
