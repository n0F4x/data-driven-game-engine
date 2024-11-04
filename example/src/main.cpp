#include <iostream>
#include <source_location>

#include <spdlog/spdlog.h>

#include <app.hpp>
#include <core/cache/Cache.hpp>
#include <plugins.hpp>

#include "demo.hpp"

static auto cache_plugin(App& app) -> void
{
    app.resources.emplace<core::cache::Cache>();
}

auto main() -> int
try {
    spdlog::set_level(spdlog::level::trace);

    const demo::ModelInfo model_info{
        std::filesystem::path{ std::source_location::current().file_name() }
                .parent_path()
                .parent_path()
            // / "models/BoxVertexColors/glTF-Binary/BoxVertexColors.glb",
            // / "models/Avocado/glTF-Binary/Avocado.glb",
            // / "models/PrimitiveModeNormalsTest/glTF/PrimitiveModeNormalsTest.gltf",
            // / "models/FlightHelmet/FlightHelmetUastc.gltf",
            // / "models/DamagedHelmet.glb",
            / "models/Sponza/glTF/Sponza.gltf",
        // / "models/StainedGlassLamp/glTF-KTX-BasisU/StainedGlassLamp.gltf",
        5
    };

    return App::create()
        .use(::cache_plugin)
        .use(plugins::Window{
            .size = { 1'280, 720 },
              .title = "My window"
    })
        .apply(plugins::Renderer{}.require_vulkan_version(1, 1))
        .run(demo::run, model_info);

} catch (const std::exception& error) {
    try {
        std::println("{}", error.what());
    } catch (...) {
        return -1;
    }
} catch (...) {
    return -2;
}
