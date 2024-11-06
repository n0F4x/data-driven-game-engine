#include <source_location>

#include <spdlog/spdlog.h>

#include <base/DemoBase.hpp>
#include <core/cache/Cache.hpp>
#include <plugins.hpp>

#include "demo.hpp"
#include "DemoApp.hpp"

[[nodiscard]]
static auto cache_plugin() -> core::cache::Cache
{
    return {};
}

auto main() -> int
try {
    spdlog::set_level(spdlog::level::trace);

    const std::filesystem::path model_filepath{
        std::filesystem::path{ std::source_location::current().file_name() }
            .parent_path()
            .parent_path()
            .parent_path()
            .parent_path()
        // / "models/BoxVertexColors/glTF-Binary/BoxVertexColors.glb",
        // / "models/Avocado/glTF-Binary/Avocado.glb",
        // / "models/PrimitiveModeNormalsTest/glTF/PrimitiveModeNormalsTest.gltf",
        // / "models/FlightHelmet/FlightHelmetUastc.gltf",
        // / "models/DamagedHelmet.glb",
        / "models/Sponza/glTF/Sponza.gltf"
        // / "models/StainedGlassLamp/glTF-KTX-BasisU/StainedGlassLamp.gltf",
    };
    constexpr static float movement_speed{ 5 };

    App::create()
        .use(::cache_plugin)
        .use(plugins::Window{
            .size  = { 1'280, 720 },
            .title = "My window",
    })
        .apply(plugins::Renderer{}.require_vulkan_version(1, 1))
        .use(examples::base::DemoBasePlugin{ .movement_speed = movement_speed })
        .use(demo::DemoPlugin{ .model_filepath = model_filepath })
        .run(demo::run);

} catch (const std::exception& error) {
    try {
        std::println("{}", error.what());
    } catch (...) {
        return -1;
    }
} catch (...) {
    return -2;
}
