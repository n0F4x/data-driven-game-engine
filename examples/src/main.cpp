#include <iostream>

#include <engine/app.hpp>
using namespace engine;

#include <engine/renderer.hpp>
#include <engine/window.hpp>

class WindowPlugin : Plugin {
public:
    auto set_context(App::Context& t_app_context) const -> void override
    {
        using namespace window;

        auto window{ Window::create()
                         .set_video_mode(sf::VideoMode{ 450u, 600u })
                         .set_title("My window")
                         .set_style(sf::Style::Default)
                         .build() };

        if (window.has_value()) {
            t_app_context.emplace<Window>(std::move(*window));
        }
    }
};

class RendererPlugin : Plugin {
public:
    auto set_context(App::Context& t_app_context) const -> void override
    {
        using namespace renderer;
        using namespace window;

        if (!t_app_context.contains<Window>()) {
            return;
        }

        auto& window{ t_app_context.get<Window>() };
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
            t_app_context.emplace<Renderer>(std::move(*renderer));
        }
    }
};

auto main() noexcept -> int
{
    auto result{ App::create()
                     .add_plugin(WindowPlugin{})
                     .add_plugin(RendererPlugin{})
                     .build_and_run([](App&) noexcept {}) };

    std::cout << (result == Result::eSuccess ? "Success" : "Failure") << '\n';
}
