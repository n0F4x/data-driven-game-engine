#include <print>

#include <app.hpp>
#include <core/window/Window.hpp>
#include <plugins.hpp>

#include "demo.hpp"

auto main() -> int
try {
    spdlog::set_level(spdlog::level::trace);

    const demo::ModelInfo model_info{
        //        "models/BoxVertexColors/glTF-Binary/BoxVertexColors.glb",
        //        "models/Avocado/glTF-Binary/Avocado.glb",
        //        "models/armor/armor.gltf",
        //        "models/DamagedHelmet.glb",
        //        "models/Sponza/glTF/Sponza.gltf",
        //        "models/SponzaKTX/Sponza01.gltf",
        "models/SponzaKTX/Sponza02.gltf",
        500,
        "shaders/pbr.frag.spv"
    };

    return App::create()
        .store<core::cache::Cache>()
        .add_plugin<plugins::Window>(core::Size2i{ 1'280, 720 }, "My window")
        .add_plugin<plugins::Renderer>()
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
