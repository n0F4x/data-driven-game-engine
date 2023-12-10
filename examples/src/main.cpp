#include <engine/app.hpp>
#include <engine/plugins.hpp>
#include <engine/window/Window.hpp>

#include "demo.hpp"

using namespace engine;

auto main() -> int
{
    return App::create()
        .add_plugin<plugins::Logger>(logger::Level::eTrace)
        .add_plugin<plugins::AssetManager>()
        .add_plugin<plugins::Window>(
            sf::VideoMode::getDesktopMode(), "My window", window::Style::eNone
        )
        .add_plugin<plugins::Renderer>(
            plugins::Renderer::default_surface_creator
        )
        .add_plugin<plugins::SceneGraph>()
        .build_and_run(demo::run);
}
