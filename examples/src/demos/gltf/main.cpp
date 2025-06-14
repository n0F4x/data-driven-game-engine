#include <filesystem>
#include <print>
#include <source_location>

#include <VkBootstrap.h>

import app;
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
    -> extensions::renderer::Requirement
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

    app::create()
        .plug_in(plugins::functional)
        .plug_in(plugins::resources)
        .plug_in(plugins::runnable)
        .inject_resource(::cache_plugin)
        .insert_resource(core::window::Window(util::Size2i{ 1'280, 720 }, "My window"))
        .transform(extensions::Renderer{}.require(::require_vulkan_version(1, 1)))
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
