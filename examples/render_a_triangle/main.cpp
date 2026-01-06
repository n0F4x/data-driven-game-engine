#include <cstdint>
#include <print>

import vulkan_hpp;

import ddge.modules.app;
import ddge.modules.renderer;
import ddge.modules.resources;
import ddge.modules.vulkan;
import ddge.modules.wsi;
import ddge.utility.containers.OptionalRef;
import ddge.utility.containers.StringLiteral;
import ddge.utility.meta.reflection.name_of;

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

    const auto application =
        app::create()
            .plug_in(app::extensions::MetaInfoPlugin{ meta_info() })
            .plug_in(resources::Plugin{})
            .plug_in(renderer::Plugin{})
            .add_render_context(
                &renderer::RenderContextBuilder::request_default_debug_messenger
            )
            .add_render_context(
                [](renderer::RenderContextBuilder& render_context_builder) -> void   //
                {
                    render_context_builder.device_builder().enable_extension(
                        vk::KHRShaderDrawParametersExtensionName
                    );
                    render_context_builder.device_builder().require_minimum_version(
                        vk::ApiVersion11
                    );
                }
            )
            .build();

    for (const auto extension :
         application.render_context.device.enabled_capabilities.extensions())
    {
        std::println("{}", extension);
    }

    if (const util::OptionalRef<const vk::PhysicalDeviceVulkan11Features> vulkan11_features =
            application.render_context.device.enabled_capabilities.features()
                .find<vk::PhysicalDeviceVulkan11Features>();
        vulkan11_features.has_value())
    {
        std::println(
            "{}: {}",
            util::meta::
                name_of<&vk::PhysicalDeviceVulkan11Features::shaderDrawParameters>(),
            vulkan11_features->shaderDrawParameters ? "VK_TRUE" : "VK_FALSE"
        );
    }

} catch (const ddge::app::BuildFailedError& error) {
    std::println("{}", error.what());
}
