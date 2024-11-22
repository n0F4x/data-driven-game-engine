module;

#include <atomic>
#include <future>
#include <functional>
#include <mutex>

#include <vulkan/vulkan.hpp>

#include <glm/ext/vector_double2.hpp>

module examples.base.DemoBase;

import core.gfx.Camera;
import core.renderer.base.device.Device;
import core.renderer.base.swapchain.SwapchainHolder;
import core.utility.Size;
import core.window.CursorMode;
import core.window.events;
import core.window.Key;
import core.window.Window;

auto examples::base::DemoBasePlugin::operator()(
    core::window::Window&                  window,
    const core::renderer::base::Device&    device,
    core::renderer::base::SwapchainHolder& swapchain_holder
) const -> DemoBase
{
    return DemoBase{
        window, Controller{ movement_speed },
         Renderer{ window, device, swapchain_holder }
    };
}

examples::base::DemoBase::DemoBase(
    core::window::Window& window,
    const Controller&     controller,
    Renderer&&            renderer
) noexcept
    : m_window{ window },
      m_controller{ controller },
      m_renderer{ std::move(renderer) }
{}

auto examples::base::DemoBase::run(
    const std::function<void(Renderer&, vk::Extent2D, core::gfx::Camera)>& render
) -> void
{
    const std::string original_window_title{ m_window.get().title() };
    bool              running{ true };

    m_window.get().set_cursor_mode(core::window::CursorMode::eDisabled);
    m_window.get().set_cursor_position(glm::dvec2{ m_window.get().size() } / 2.0);

    bool reset_mouse{};

    std::atomic<vk::Extent2D> framebuffer_size{};
    m_window.get().set_framebuffer_size_callback(
        [&framebuffer_size](const core::Size2i size) {
            framebuffer_size.store(static_cast<vk::Extent2D>(size));
        }
    );

    core::gfx::Camera camera;
    std::mutex        camera_mutex{};

    std::atomic_uint16_t fps{};

    std::optional<std::future<void>>
        rendering{ render == nullptr
                       ? std::nullopt
                       : std::optional{ std::async(std::launch::async, [&] {
                             std::chrono::time_point last_time{
                                 std::chrono::high_resolution_clock::now()
                             };
                             while (running) {
                                 const std::chrono::time_point now{
                                     std::chrono::high_resolution_clock::now()
                                 };
                                 const std::chrono::duration<double> delta_time{
                                     now - last_time
                                 };
                                 last_time = now;
                                 fps = static_cast<uint16_t>(1.0 / delta_time.count());

                                 camera_mutex.lock();
                                 const core::gfx::Camera render_camera{ camera };
                                 camera_mutex.unlock();
                                 if (render) {
                                     render(m_renderer, framebuffer_size, render_camera);
                                 }
                             }
                         }) } };

    std::chrono::time_point last_time{ std::chrono::high_resolution_clock::now() };
    while (running) {
        core::window::poll_events();

        const std::chrono::time_point now{ std::chrono::high_resolution_clock::now() };
        const std::chrono::duration<double> delta_time{ now - last_time };
        last_time = now;

        const std::string new_window_title{ original_window_title
                                            + " - FPS: " + std::to_string(fps) };
        m_window.get().set_title(new_window_title.c_str());

        if (m_window.get().should_close()) {
            running = false;
        }

        if (m_window.get().key_pressed(core::window::eEscape)) {
            running = false;
        }

        if (m_window.get().key_pressed(core::window::eLeftControl)) {
            m_window.get().set_cursor_mode(core::window::CursorMode::eNormal);
            reset_mouse = true;
        }
        else {
            m_window.get().set_cursor_mode(core::window::CursorMode::eDisabled);

            if (!reset_mouse) {
                m_controller.update(m_window.get(), delta_time);
                camera_mutex.lock();
                camera = m_controller.update_camera(camera);
                camera_mutex.unlock();
            }

            m_window.get().set_cursor_position(glm::dvec2{ m_window.get().size() } / 2.0);

            reset_mouse = false;
        }

        std::this_thread::sleep_for(std::chrono::duration<double>{ 1.0 / 60.0 });
    }

    if (rendering.has_value()) {
        rendering.value().get();
    }
    m_renderer.wait_idle();
}
