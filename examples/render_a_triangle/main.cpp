#include <chrono>
#include <cstdint>
#include <print>
#include <thread>

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.renderer;
import ddge.modules.resources;
import ddge.modules.wsi;
import ddge.utility.containers.StringLiteral;

constexpr static ddge::util::StringLiteral application_name{ "render_a_triangle" };
constexpr static uint32_t                  application_version{ 0 };

[[nodiscard]]
constexpr auto meta_info() -> ddge::app::extensions::MetaInfo
{
    return ddge::app::extensions::MetaInfo{
        application_name,
        application_version,
    };
}

auto main() -> int
try {
    using namespace ddge;

    static const wsi::Context context;

    auto application =
        app::create()
            .plug_in(app::extensions::MetaInfoPlugin{ meta_info() })
            .plug_in(resources::Plugin{})
            .plug_in(renderer::Plugin{})
            .add_render_context(
                &renderer::RenderContextBuilder::request_default_debug_messenger
            )
            .build();

    constexpr wsi::WindowedWindowSettings screen_settings{
        .content_size{ .width = 640, .height = 480 }
    };
    constexpr wsi::Window::CreateInfo window_info{
        .title    = "Hello Window!",
        .settings = screen_settings,
    };

    wsi::VulkanWindow window{
        wsi::Window{ auto{ context }, window_info },
        application.render_context.instance,
        application.render_context.device,
        1
    };

    std::this_thread::sleep_for(std::chrono::seconds{ 2 });

} catch (const ddge::app::BuildFailedError& error) {
    std::println("{}", error.what());
}
