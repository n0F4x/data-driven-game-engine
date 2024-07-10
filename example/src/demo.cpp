#include "demo.hpp"

#include <atomic>
#include <future>
#include <mutex>
#include <thread>

#include <core/window/events.hpp>
#include <core/window/Window.hpp>

#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/swapchain/SwapchainHolder.hpp"

#include "Controller.hpp"
#include "demo_init.hpp"
#include "DemoRenderer.hpp"

using namespace entt::literals;
using namespace core;

auto demo::run(App t_app, const ModelInfo& t_model_info) -> int
{
    return DemoRenderer::create(
               t_app.store(), t_model_info.filepath, t_model_info.fragment_shader
    )
        .transform([&](DemoRenderer t_demo) {
            t_demo.swapchain.get().on_swapchain_recreated(
                [&t_demo](const renderer::Swapchain& t_swapchain) {
                    t_demo.depth_image.reset();
                    t_demo.depth_image = init::create_depth_image(
                        t_demo.device.get().physical_device(),
                        t_demo.allocator,
                        t_swapchain.extent()
                    );
                }
            );
            t_demo.swapchain.get().on_swapchain_recreated(
                [&t_demo](const renderer::Swapchain&) {
                    t_demo.depth_image_view.reset();
                    t_demo.depth_image_view = init::create_depth_image_view(
                        t_demo.device, t_demo.depth_image.get()
                    );
                }
            );
            t_demo.swapchain.get().on_swapchain_recreated(
                [&t_demo](const renderer::Swapchain& t_swapchain) {
                    t_demo.framebuffers.clear();
                    t_demo.framebuffers = init::create_framebuffers(
                        t_demo.device.get().get(),
                        t_swapchain.extent(),
                        t_swapchain.image_views(),
                        t_demo.render_pass.get(),
                        t_demo.depth_image_view.get()
                    );
                }
            );

            bool  running{ true };
            auto& window{ t_app.store().at<window::Window>() };

            window.set_cursor_mode(core::window::CursorMode::eDisabled);
            window.set_cursor_position(glm::dvec2{ window.size() } / 2.0);

            Controller controller{ t_model_info.movement_speed };
            bool       reset_mouse{};

            std::atomic<vk::Extent2D> framebuffer_size{};
            window.set_framebuffer_size_callback([&framebuffer_size](Size2i size) {
                framebuffer_size.store(static_cast<vk::Extent2D>(size));
            });

            graphics::Camera camera;
            std::mutex       camera_mutex{};

            auto rendering = std::async(std::launch::async, [&] {
                while (running) {
                    camera_mutex.lock();
                    const graphics::Camera render_camera{ camera };
                    camera_mutex.unlock();
                    t_demo.render(framebuffer_size, render_camera);
                }
            });

            std::chrono::time_point last_time{ std::chrono::high_resolution_clock::now() };
            while (running) {
                std::this_thread::sleep_for(std::chrono::duration<double>{ 1.0 / 60.0 });
                const std::chrono::time_point now{
                    std::chrono::high_resolution_clock::now()
                };
                const std::chrono::duration<double> delta_time{ now - last_time };
                last_time = now;

                core::window::poll_events();

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
            }

            rendering.get();
            t_demo.device.get()->waitIdle();

            return 0;
        })
        .value_or(1);
}
