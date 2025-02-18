#include <filesystem>
#include <print>
#include <source_location>

#include <VkBootstrap.h>

import core;
import addons;
import extensions;
import plugins;

import utility.Size;

import examples.base.DemoBase;
import demos.gltf.DemoApp;
import demos.gltf;

[[nodiscard]]
static auto cache_plugin() -> core::cache::Cache
{
    return {};
}

[[nodiscard]]
static auto require_vulkan_version(const uint32_t major, const uint32_t minor)
    -> plugins::renderer::Requirement
{
    return { .enable_instance_settings = [=](const vkb::SystemInfo&,
                                             vkb::InstanceBuilder& instance_builder) {
        instance_builder.require_api_version(major, minor);
    } };
}

auto main() -> int
try {
    const std::filesystem::path model_filepath{
        std::filesystem::path{ std::source_location::current().file_name() }
            .parent_path()
            .parent_path()
            .parent_path()
            .parent_path()
        / "assets/models"
        // / "BoxVertexColors/glTF-Binary/BoxVertexColors.glb",
        // / "Avocado/glTF-Binary/Avocado.glb",
        // / "PrimitiveModeNormalsTest/glTF/PrimitiveModeNormalsTest.gltf",
        // / "FlightHelmet/FlightHelmetUastc.gltf",
        // / "DamagedHelmet.glb",
        / "Sponza/glTF/Sponza.gltf"
        // / "StainedGlassLamp/glTF-KTX-BasisU/StainedGlassLamp.gltf",
    };
    constexpr static float movement_speed{ 10 };

    core::config::logging::set_level(core::config::logging::eTrace);

    core::app::create()
        .extend_with<extensions::Functional>()
        .extend_with<extensions::ResourceManager>()
        .extend_with<extensions::Runnable>()
        .inject_resource(::cache_plugin)
        .use_resource(core::window::Window(util::Size2i{ 1'280, 720 }, "My window"))
        .transform(plugins::Renderer{}.require(::require_vulkan_version(1, 1)))
        .inject_resource(
            examples::base::DemoBasePlugin{ .movement_speed = movement_speed }
        )
        .inject_resource(
            demo::DemoPlugin{
                .model_filepath     = model_filepath,
                .use_virtual_images = true,
            }
        )
        .run(demo::run);

} catch (const std::exception& error) {
    try {
        std::println("{}", error.what());
        return -1;
    } catch (...) {
        return -3;
    }
} catch (...) {
    return -2;
}
