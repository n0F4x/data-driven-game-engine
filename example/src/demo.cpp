#include "demo.hpp"

#include <atomic>
#include <future>
#include <mutex>
#include <thread>

#include <core/window/Window.hpp>

#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/swapchain/SwapchainHolder.hpp"

#include "Controller.hpp"
#include "demo_init.hpp"
#include "DemoRenderer.hpp"

using namespace entt::literals;
using namespace core;

auto demo::run(app::App& t_app, const ModelInfo& t_model_info) noexcept -> int
{
    return DemoRenderer::create(
               t_app.store(), t_model_info.filepath, t_model_info.fragment_shader
    )
        .transform([&](DemoRenderer t_demo) {
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const renderer::Swapchain& t_swapchain) {
                    t_demo.depth_image.reset();
                    t_demo.depth_image = init::create_depth_image(
                        t_demo.device.physical_device(),
                        t_demo.allocator,
                        t_swapchain.extent()
                    );
                }
            );
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const renderer::Swapchain&) {
                    t_demo.depth_image_view.reset();
                    t_demo.depth_image_view =
                        init::create_depth_image_view(t_demo.device, *t_demo.depth_image);
                }
            );
            t_demo.swapchain.on_swapchain_recreated(
                [&t_demo](const renderer::Swapchain& t_swapchain) {
                    t_demo.framebuffers.clear();
                    t_demo.framebuffers = init::create_framebuffers(
                        *t_demo.device,
                        t_swapchain.extent(),
                        t_swapchain.image_views(),
                        *t_demo.render_pass,
                        *t_demo.depth_image_view
                    );
                }
            );

            bool        running{ true };
            const auto& window{ t_app.store().at<window::Window>() };

            glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(window.get(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            int window_width{};
            int window_height{};
            glfwGetWindowSize(window.get(), &window_width, &window_height);
            glfwSetCursorPos(
                window.get(),
                static_cast<double>(window_width) / 2.0,
                static_cast<double>(window_height) / 2.0
            );
            Controller controller{ t_model_info.movement_speed };
            bool       reset_mouse{};

            std::atomic<vk::Extent2D> framebuffer_size{};
            glfwSetWindowUserPointer(window.get(), &framebuffer_size);
            glfwSetFramebufferSizeCallback(
                window.get(),
                [](GLFWwindow* raw_window, const int width, const int height) {
                    static_cast<std::atomic<vk::Extent2D>*>(
                        glfwGetWindowUserPointer(raw_window)
                    )
                        ->store(vk::Extent2D{ static_cast<uint32_t>(width),
                                              static_cast<uint32_t>(height) });
                }
            );
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

            auto last_time = std::chrono::high_resolution_clock::now();
            while (running) {
                std::this_thread::sleep_for(std::chrono::duration<float>{ 1.f / 60.f });
                const auto now = std::chrono::high_resolution_clock::now();
                const std::chrono::duration<float> delta_time{ now - last_time };
                last_time = now;

                glfwPollEvents();

                if (glfwWindowShouldClose(window.get()) == GLFW_TRUE) {
                    running = false;
                }

                if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    running = false;
                }

                if (glfwGetKey(window.get(), GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
                    glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    glfwSetInputMode(window.get(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

                    if (!reset_mouse) {
                        controller.update(window, delta_time.count());
                        camera_mutex.lock();
                        camera = controller.update_camera(camera);
                        camera_mutex.unlock();
                    }

                    glfwGetWindowSize(window.get(), &window_width, &window_height);
                    glfwSetCursorPos(
                        window.get(),
                        static_cast<double>(window_width) / 2.0,
                        static_cast<double>(window_height) / 2.0
                    );

                    reset_mouse = false;
                }
                if (glfwGetKey(window.get(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                    glfwSetInputMode(window.get(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
                    glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    reset_mouse = true;
                }
            }

            rendering.get();
            t_demo.device->waitIdle();

            return 0;
        })
        .value_or(1);
}
