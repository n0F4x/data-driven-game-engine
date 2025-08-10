module;

#include <SFML/Graphics.hpp>

export module snake.window.clear_window;

import ddge.modules.scheduler.accessors.resources;

import snake.window.Window;

using namespace ddge::scheduler::accessors;

namespace window {

export auto clear_window(resources::Resource<Window> window) -> void;

}   // namespace window

module :private;

auto window::clear_window(const resources::Resource<Window> window) -> void
{
    window->clear(sf::Color{ 128, 0, 128 });
}
