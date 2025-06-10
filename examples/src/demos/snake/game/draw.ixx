module;

#include <SFML/Graphics.hpp>

export module snake.game.draw;

import core.ecs;
import core.scheduler;

import extensions.scheduler.accessors.ecs.Query;
import extensions.scheduler.accessors.resources;

import snake.game.Cell;
import snake.window.Window;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export inline constexpr auto draw =
    [](resources::Ref<window::Window>                   window,
       ecs::Query<With<Cell>, const sf::RectangleShape> cells)   //
{                                                                //
    cells.for_each([window](const sf::RectangleShape& shape) { window->draw(shape); });
};

}   // namespace game
