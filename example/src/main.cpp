#include <print>

#include <app.hpp>
#include <core/window/Window.hpp>
#include <plugins.hpp>

#include "demo.hpp"

struct A {
    int i;
};

auto main() -> int
try {
    const std::string model_file_path{
        //    "models/BoxVertexColors/glTF-Binary/BoxVertexColors.glb"
        //    "models/Avocado/glTF-Binary/Avocado.glb"
        //    "models/DamagedHelmet.glb"
        //    "models/Sponza/glTF/Sponza.gltf"
        "models/armor/armor.gltf"
    };

    return app::App::create()
        .add_plugin<plugins::Logger>(plugins::Logger::Level::eTrace)
        .add_plugin<plugins::Cache>()
        .add_plugin<plugins::Window>(
            1'280, 720, "My window", plugins::Window::default_configure
        )
        .add_plugin<plugins::Renderer>(plugins::Renderer::create_default_surface)
        .build_and_run(demo::run, model_file_path);
} catch (std::exception& error) {
    std::println("{}", error.what());
} catch (...) {
}
