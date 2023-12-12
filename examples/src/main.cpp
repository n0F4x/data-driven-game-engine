#include <engine/app.hpp>
#include <engine/plugins.hpp>
#include <engine/window/Window.hpp>

#include "demo.hpp"

using namespace engine;

// const std::string g_model_file_path{
//     "models/BoxVertexColors/glTF-Binary/BoxVertexColors.glb"
// };
//  const std::string g_model_file_path{
//  "models/Avocado/glTF-Binary/Avocado.glb"
//  };
const std::string g_model_file_path{ "models/DamagedHelmet.glb" };

// const std::string g_model_file_path{ "models/Sponza/glTF/Sponza.gltf" };

auto main() -> int
{
    return App::create()
        .add_plugin<plugins::Logger>(logger::Level::eTrace)
        .add_plugin<plugins::AssetManager>()
        .add_plugin<plugins::Window>(
            sf::VideoMode{ 1'080, 720 }, "My window", window::Style::eDefault
        )
        .add_plugin<plugins::Renderer>(
            plugins::Renderer::default_surface_creator
        )
        .add_plugin<plugins::SceneGraph>()
        .build_and_run(demo::run, g_model_file_path);
}
