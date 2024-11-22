module;

#include <core/renderer/base/device/Device.hpp>
#include <core/renderer/base/swapchain/SwapchainHolder.hpp>

export module examples.base.DemoBase;

import core.gfx.Camera;
import core.window.Window;

import examples.base.Controller;
import examples.base.Renderer;

namespace examples::base {

export class DemoBase;

export struct DemoBasePlugin {
    float movement_speed{ 5.f };

    [[nodiscard]]
    auto operator()(
        core::window::Window&                  window,
        const core::renderer::base::Device&    device,
        core::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoBase;
};

export class DemoBase {
public:
    DemoBase(
        core::window::Window& window,
        const Controller&     controller,
        Renderer&&            renderer
    ) noexcept;

    auto run(const std::function<void(Renderer&, vk::Extent2D, core::gfx::Camera)>& render)
        -> void;

private:
    std::reference_wrapper<core::window::Window> m_window;
    Controller                                   m_controller;
    Renderer                                     m_renderer;
};

}   // namespace examples::base
