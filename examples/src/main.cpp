#include <engine/app.hpp>
#include <engine/asset_manager/GltfLoader.hpp>
#include <engine/graphics/ModelFactory.hpp>
#include <engine/plugins.hpp>
using namespace engine;

auto main() noexcept -> int
{
    return App::create()
        .add_plugin<plugins::Logger>(logger::Level::eTrace)
        .add_plugin<plugins::AssetManager>()
        .add_plugin<plugins::Window>(
            sf::VideoMode{ 450u, 600u }, "My window", window::Style::eDefault
        )
        .add_plugin<plugins::Renderer>()
        .add_plugin<plugins::SceneGraph>()
        .build_and_run([](App&) noexcept { return 0; })
        .value_or(1);
}
