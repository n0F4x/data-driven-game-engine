#include <cassert>
#include <cstdint>
#include <print>

import vulkan_hpp;

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
try {
    using namespace ddge;

    auto application =
        app::create()
            .plug_in(app::extensions::MetaInfoPlugin{ meta_info() })
            .plug_in(renderer::Plugin{})
            .add_render_context(
                [](renderer::RenderContextBuilder& render_context_builder) -> void {
                    [[maybe_unused]]
                    const bool success =
                        render_context_builder.request_default_debug_messenger();
                    assert(success);

                    render_context_builder.device_builder().enable_features(
                        vk::PhysicalDeviceAccelerationStructureFeaturesKHR{
                            .accelerationStructureCaptureReplay = true,
                        }
                    );
                    render_context_builder.device_builder().enable_features_if_available(
                        vk::PhysicalDeviceAccelerationStructureFeaturesKHR{
                            .accelerationStructureCaptureReplay = true,
                        }
                    );
                }
            )
            .build();
} catch (const ddge::app::BuildFailedError& error) {
    std::println("{}", error.what());
}
