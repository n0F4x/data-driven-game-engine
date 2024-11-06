#pragma once

#include "Controller.hpp"
#include "Renderer.hpp"

namespace examples::base {

class DemoBase;

struct DemoBasePlugin {
    float movement_speed{ 5.f };

    [[nodiscard]]
    auto operator()(
        core::window::Window&                  window,
        const core::renderer::base::Device&    device,
        core::renderer::base::SwapchainHolder& swapchain_holder
    ) const -> DemoBase;
};

class DemoBase {
public:
    DemoBase(
        core::window::Window& window,
        const Controller&     controller,
        Renderer&&            renderer
    ) noexcept;

    auto run(const std::function<void(Renderer&, vk::Extent2D, core::gfx::Camera)>& render
    ) -> void;

private:
    std::reference_wrapper<core::window::Window> m_window;
    Controller                                   m_controller;
    Renderer                                     m_renderer;
};

}   // namespace examples::base
