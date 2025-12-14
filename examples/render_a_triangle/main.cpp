#include <cassert>
#include <cstdint>
#include <print>

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.renderer;
import ddge.modules.vulkan;

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

    auto instance =
        vulkan::InstanceBuilder{
            vulkan::InstanceBuilder::CreateInfo{},
            vulkan::context(),
        }
            .build();

    vulkan::PhysicalDeviceSelector physical_device_selector;
    physical_device_selector.require_features(
        vk::PhysicalDeviceAccelerationStructureFeaturesKHR{
            .accelerationStructureCaptureReplay = true,
        }
    );
    auto fitting_devices{ physical_device_selector.select_devices(instance) };
    std::println(
        "{}",
        static_cast<const char*>(
            fitting_devices.front().getProperties2().properties.deviceName
        )
    );

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
