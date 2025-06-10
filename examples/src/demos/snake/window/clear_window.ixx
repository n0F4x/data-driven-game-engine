module;

#include <SFML/Graphics.hpp>

export module snake.window.clear_window;

import extensions.scheduler.accessors.resources;

import snake.window.Window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto clear_window =     //
    [](const resources::Ref<Window> window) {   //
        window->clear(sf::Color{ 128, 0, 128 });
    };

}   // namespace window
