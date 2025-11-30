module;

#include <SFML/Graphics.hpp>

module snake.window.tasks.clear_window;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.primitives.force_on_main;

import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::tasks::clear_window()
    -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::force_on_main(
        +[](const resources::Resource<Window> window) -> void {
            window->clear(sf::Color{ 128, 0, 128 });
        }   //
    );
}
