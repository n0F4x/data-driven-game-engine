module;

#include <functional>

#include <vulkan/vulkan.hpp>

export module examples.base.DemoBase;

import modules.gfx.Camera;
import modules.renderer.base.device.Device;
import modules.renderer.base.swapchain.SwapchainHolder;
import modules.window.Window;

import examples.base.Controller;
import examples.base.Renderer;

namespace examples::base {

export class DemoBase;

export struct DemoBasePlugin {
    float movement_speed{ 5.f };

    [[nodiscard]]
    auto operator()(
        modules::window::Window&                  window,
        const modules::renderer::base::Device&    device,
        modules::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoBase;
};

export class DemoBase {
public:
    DemoBase(
        modules::window::Window& window,
        const Controller&     controller,
        Renderer&&            renderer
    ) noexcept;

    auto run(const std::function<void(Renderer&, vk::Extent2D, modules::gfx::Camera)>& render)
        -> void;

private:
    std::reference_wrapper<modules::window::Window> m_window;
    Controller                                   m_controller;
    Renderer                                     m_renderer;
};

}   // namespace examples::base
