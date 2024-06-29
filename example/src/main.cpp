#include <print>

#include <app.hpp>
#include <core/window/Window.hpp>
#include <plugins.hpp>

#include "demo.hpp"

auto main() -> int
try {
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
        .add_plugin<plugins::Logger>(plugins::Logger::Level::eTrace)
        .add_plugin<plugins::Cache>()
        .add_plugin<plugins::Window>(1'280, 720, "My window")
        .add_plugin<plugins::Renderer>()
        .build_and_run(demo::run, model_info);
} catch (std::exception& error) {
    try {
        std::println("{}", error.what());
    } catch (...) {
        return -1;
    }
} catch (...) {
    return -2;
}
