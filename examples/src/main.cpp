#include <iostream>

#include "engine/engine.hpp"

auto main() noexcept -> int
{
    using namespace engine;

    auto result{
        App::create()
            .add_plugin(plugins::ResourceManager{}
            )
            .add_plugin(plugins::Window{ sf::VideoMode{ 450u, 600u },
                                                 "My window", window::Window::Style::eDefault }
            )
            .add_plugin(plugins::Renderer{}
            )
            .add_plugin(plugins::SceneGraph{}
            )
            .build_and_run([](App&) noexcept {}
            )
    };

    std::cout << (result == Result::eSuccess ? "Success" : "Failure") << '\n';
}
