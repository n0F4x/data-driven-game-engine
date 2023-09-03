#include <iostream>

#include "engine/engine.hpp"

auto main() noexcept -> int
{
    using namespace engine;
    using namespace renderer;
    using namespace window;

    auto result{
        App::create()
            .add_plugin(WindowPlugin{ sf::VideoMode{ 450u, 600u },
                                     "My window", Window::Style::eDefault }
            )
            .add_plugin(RendererPlugin{}
            )
            .build_and_run([](App&) noexcept {}
            )
    };

    std::cout << (result == Result::eSuccess ? "Success" : "Failure") << '\n';
}
