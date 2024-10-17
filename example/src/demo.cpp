#include "demo.hpp"

#include <atomic>
#include <future>
#include <thread>

#include <core/window/events.hpp>
#include <core/window/Window.hpp>

#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/swapchain/SwapchainHolder.hpp"

#include "Controller.hpp"
#include "init.hpp"
#include "Renderer.hpp"

using namespace entt::literals;

auto demo::run(App app, const ModelInfo& model_info) -> int
{
    return DemoRenderer::create(app.resources, model_info.filepath)
        .transform([&](DemoRenderer demo) {
            demo.swapchain.get().on_swapchain_recreated(
                [&demo](const core::renderer::base::Swapchain& swapchain) {
                    demo.depth_image.reset();
                    demo.depth_image = init::create_depth_image(
                        demo.device.get().physical_device(),
                        demo.allocator,
                        swapchain.extent()
                    );
                }
            );
            demo.swapchain.get().on_swapchain_recreated(
                [&demo](const core::renderer::base::Swapchain&) {
                    demo.depth_image_view.reset();
                    demo.depth_image_view = init::create_depth_image_view(
                        demo.device, demo.depth_image.get()
                    );
                }
            );
            demo.swapchain.get().on_swapchain_recreated(
                [&demo](const core::renderer::base::Swapchain& swapchain) {
                    demo.framebuffers.clear();
                    demo.framebuffers = init::create_framebuffers(
                        demo.device.get().get(),
                        swapchain.extent(),
                        swapchain.image_views(),
                        demo.render_pass.get(),
                        demo.depth_image_view.get()
                    );
                }
            );

            bool  running{ true };
            auto& window{ app.resources.at<core::window::Window>() };

            window.set_cursor_mode(core::window::CursorMode::eDisabled);
            window.set_cursor_position(glm::dvec2{ window.size() } / 2.0);

            Controller controller{ model_info.movement_speed };
            bool       reset_mouse{};

            std::atomic<vk::Extent2D> framebuffer_size{};
            window.set_framebuffer_size_callback(
                [&framebuffer_size](const core::Size2i size) {
                    framebuffer_size.store(static_cast<vk::Extent2D>(size));
                }
            );

            core::graphics::Camera camera;
            std::mutex             camera_mutex{};

            auto rendering = std::async(std::launch::async, [&] {
                while (running) {
                    camera_mutex.lock();
                    const core::graphics::Camera render_camera{ camera };
                    camera_mutex.unlock();
                    demo.render(framebuffer_size, render_camera);
                }
            });

            std::chrono::time_point last_time{ std::chrono::high_resolution_clock::now() };
            while (running) {
                core::window::poll_events();

                const std::chrono::time_point now{
                    std::chrono::high_resolution_clock::now()
                };
                const std::chrono::duration<double> delta_time{ now - last_time };
                last_time = now;

                if (window.should_close()) {
                    running = false;
                }

                if (window.key_pressed(core::window::eEscape)) {
                    running = false;
                }

                if (window.key_pressed(core::window::eLeftControl)) {
                    window.set_cursor_mode(core::window::CursorMode::eNormal);
                    reset_mouse = true;
                }
                else {
                    window.set_cursor_mode(core::window::CursorMode::eDisabled);

                    if (!reset_mouse) {
                        controller.update(window, delta_time);
                        camera_mutex.lock();
                        camera = controller.update_camera(camera);
                        camera_mutex.unlock();
                    }

                    window.set_cursor_position(glm::dvec2{ window.size() } / 2.0);

                    reset_mouse = false;
                }

                std::this_thread::sleep_for(std::chrono::duration<double>{ 1.0 / 60.0 });
            }

            rendering.get();
            demo.device.get()->waitIdle();

            return 0;
        })
        .value_or(1);
}
