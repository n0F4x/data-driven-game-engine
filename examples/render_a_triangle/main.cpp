#include <cstdint>
#include <print>

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.renderer;
import ddge.modules.resources;
import ddge.modules.wsi;
import ddge.utility.containers.StringLiteral;

using namespace ddge;

constexpr static util::StringLiteral application_name{ "render_a_triangle" };
constexpr static uint32_t            application_version{ 0 };

[[nodiscard]]
constexpr auto meta_info() -> app::extensions::MetaInfo
{
    return app::extensions::MetaInfo{
        application_name,
        application_version,
    };
}

using App = app::App<app::MetaInfoAddon, resources::Addon>;

auto run(const App& app) -> void;

auto main() -> int
try {
    const App app = app::create()
                        .plug_in(app::extensions::MetaInfoPlugin{ meta_info() })
                        .plug_in(resources::Plugin{})
                        .plug_in(renderer::Plugin{})
                        .add_render_context(
                            &renderer::ContextInjection::request_default_debug_messenger
                        )
                        .build();

    run(app);

} catch (const app::BuildFailedError& error) {
    std::println("{}", error.what());
}

auto run(const App& app) -> void
{
    const wsi::Context&      wsi_context{ app.resource_manager.at<wsi::Context>() };
    const renderer::Context& render_context{
        app.resource_manager.at<renderer::Context>()
    };

    constexpr wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    constexpr wsi::Window::CreateInfo window_info{
        .title    = "Hello Window!",
        .settings = screen_settings,
    };

    wsi::VulkanWindow window{
        wsi::Window{ auto{ wsi_context }, window_info },
        render_context.instance,
        render_context.device,
        1
    };

    while (!window.should_close()) {
        wsi::poll_events(wsi_context);

        if (window.key_pressed(wsi::Key::eEscape)) {
            window.request_close();
        }
    }
}
