module;

#include <atomic>
#include <functional>
#include <future>
#include <mutex>
#include <thread>

#include <vulkan/vulkan.hpp>

#include <glm/ext/vector_double2.hpp>

module examples.base.DemoBase;

import ddge.modules.gfx.Camera;
import ddge.modules.renderer.base.device.Device;
import ddge.modules.renderer.base.swapchain.SwapchainHolder;
import ddge.utility.Size;
import ddge.modules.window.CursorMode;
import ddge.modules.window.events;
import ddge.modules.window.Key;
import ddge.modules.window.Window;

auto examples::base::DemoBasePlugin::operator()(
    ddge::window::Window&                  window,
    const ddge::renderer::base::Device&    device,
    ddge::renderer::base::SwapchainHolder& swapchain_holder
) const -> DemoBase
{
    return DemoBase{
        window, Controller{ movement_speed },
         Renderer{ window, device, swapchain_holder }
    };
}

examples::base::DemoBase::DemoBase(
    ddge::window::Window& window,
    const Controller&     controller,
    Renderer&&            renderer
) noexcept
    : m_window{ window },
      m_controller{ controller },
      m_renderer{ std::move(renderer) }
{}

auto examples::base::DemoBase::run(
    const std::function<void(Renderer&, vk::Extent2D, ddge::gfx::Camera)>& render
) -> void
{
    const std::string original_window_title{ m_window.get().title() };
    bool              running{ true };

    m_window.get().set_cursor_mode(ddge::window::CursorMode::eDisabled);
    m_window.get().set_cursor_position(glm::dvec2{ m_window.get().size() } / 2.0);

    bool reset_mouse{};

    std::atomic<vk::Extent2D> framebuffer_size{};
    m_window.get().set_framebuffer_size_callback(
        [&framebuffer_size](const ddge::util::Size2i size) {
            framebuffer_size.store(static_cast<vk::Extent2D>(size));
        }
    );

    ddge::gfx::Camera camera;
    std::mutex        camera_mutex{};

    std::atomic_uint32_t fps{};

    std::optional<std::future<void>>
        rendering{ render == nullptr
                       ? std::nullopt
                       : std::optional{ std::async(std::launch::async, [&] {
                             std::chrono::time_point last_time{
                                 std::chrono::high_resolution_clock::now()
                             };
                             while (running) {
                                 using namespace std::chrono_literals;

                                 const std::chrono::time_point now{
                                     std::chrono::high_resolution_clock::now()
                                 };
                                 const std::chrono::duration<double> delta_time{
                                     now - last_time
                                 };
                                 last_time = now;
                                 fps       = static_cast<uint32_t>(1s / delta_time);

                                 camera_mutex.lock();
                                 const ddge::gfx::Camera render_camera{ camera };
                                 camera_mutex.unlock();
                                 if (render) {
                                     render(m_renderer, framebuffer_size, render_camera);
                                 }
                             }
                         }) } };

    auto rendering_finished{ [&rendering] {
        return rendering
            && rendering.value().wait_for(std::chrono::seconds{ 0 })
                   == std::future_status::ready;
    } };

    std::chrono::time_point last_time{ std::chrono::high_resolution_clock::now() };
    while (running && !rendering_finished()) {
        ddge::window::poll_events();

        const std::chrono::time_point now{ std::chrono::high_resolution_clock::now() };
        const std::chrono::duration<double> delta_time{ now - last_time };
        last_time = now;

        const std::string new_window_title{ original_window_title
                                            + " - FPS: " + std::to_string(fps) };
        m_window.get().set_title(new_window_title.c_str());

        if (m_window.get().should_close()) {
            running = false;
        }

        if (m_window.get().key_pressed(ddge::window::eEscape)) {
            running = false;
        }

        if (m_window.get().key_pressed(ddge::window::eLeftControl)) {
            m_window.get().set_cursor_mode(ddge::window::CursorMode::eNormal);
            reset_mouse = true;
        }
        else {
            m_window.get().set_cursor_mode(ddge::window::CursorMode::eDisabled);

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
