module;

#include <functional>

#include <vulkan/vulkan.hpp>

export module examples.base.DemoBase;

import ddge.modules.gfx.Camera;
import ddge.modules.renderer.base.device.Device;
import ddge.modules.renderer.base.swapchain.SwapchainHolder;
import ddge.modules.window.Window;

import examples.base.Controller;
import examples.base.Renderer;

namespace examples::base {

export class DemoBase;

export struct DemoBasePlugin {
    float movement_speed{ 5.f };

    [[nodiscard]]
    auto operator()(
        ddge::window::Window&                  window,
        const ddge::renderer::base::Device&    device,
        ddge::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoBase;
};

export class DemoBase {
public:
    DemoBase(
        ddge::window::Window& window,
        const Controller&     controller,
        Renderer&&            renderer
    ) noexcept;

    auto run(const std::function<void(Renderer&, vk::Extent2D, ddge::gfx::Camera)>& render)
        -> void;

private:
    std::reference_wrapper<ddge::window::Window> m_window;
    Controller                                   m_controller;
    Renderer                                     m_renderer;
};

}   // namespace examples::base
