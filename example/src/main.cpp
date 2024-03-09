#include <engine/app.hpp>
#include <engine/plugins.hpp>
#include <engine/window/Window.hpp>
// #include <ktx.h>
// #include <ktxvulkan.h>

#include "demo.hpp"

using namespace engine;

const std::string g_model_file_path{
    //    "models/BoxVertexColors/glTF-Binary/BoxVertexColors.glb"
    //    "models/Avocado/glTF-Binary/Avocado.glb"
    //    "models/DamagedHelmet.glb"
    "models/Sponza/glTF/Sponza.gltf"
};

auto main() -> int
{
    return App::create()
        .add_plugin<plugins::Logger>(logger::Level::eTrace)
        .add_plugin<plugins::Common>()
        .add_plugin<plugins::Window>(
            1'280, 720, "My window", plugins::Window::default_configure
        )
        .add_plugin<plugins::Renderer>(plugins::Renderer::create_default_surface)
        .build_and_run(demo::run, g_model_file_path);
}
