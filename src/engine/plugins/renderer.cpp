#include "renderer.hpp"

#include "engine/plugins/window/Window.hpp"

namespace engine::renderer {

auto RendererPlugin::set_context(App::Context& t_context) const noexcept -> void
{
    using namespace window;

    if (!t_context.contains<Window>()) {
        return;
    }

    auto& window{ t_context.get<Window>() };
    auto  framebuffer_size{ [&]() {
        auto framebuffer_size{ window.framebuffer_size() };
        return vk::Extent2D{ .width  = framebuffer_size.x,
                             .height = framebuffer_size.y };
    }() };

    auto renderer{ Renderer::create_default(
        [&](vk::Instance instance) noexcept {
            return window.create_vulkan_surface(instance);
        },
        framebuffer_size
    ) };
    if (renderer.has_value()) {
        t_context.emplace<Renderer>(std::move(*renderer));
    }
}

}   // namespace engine::renderer
