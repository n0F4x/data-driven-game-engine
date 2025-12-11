#include <cassert>
#include <cstdint>

import ddge.modules.app;
import ddge.modules.renderer;

constexpr static const char* application_name{ "render_a_triangle" };
constexpr static uint32_t    application_version{ 0 };

[[nodiscard]]
constexpr auto meta_info() -> ddge::app::extensions::MetaInfo
{
    return ddge::app::extensions::MetaInfo{
        application_name,
        application_version,
    };
}

auto main() -> int
{
    using namespace ddge;

    auto application =
        app::create()
            .plug_in(app::extensions::MetaInfoPlugin{ meta_info() })
            .plug_in(renderer::Plugin{})
            .add_render_context(
                [](renderer::RenderContextBuilder& render_context_builder) -> void {
                    [[maybe_unused]]
                    const bool success =
                        render_context_builder.instance_builder().enable_vulkan_layer(
                            "VK_LAYER_KHRONOS_validation"
                        )
                        && render_context_builder.request_default_debug_messenger();
                    assert(success);
                }
            )
            .build();
}
