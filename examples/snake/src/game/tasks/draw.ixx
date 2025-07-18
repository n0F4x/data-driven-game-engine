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

export inline constexpr auto draw = [](resources::Resource<window::Window> window,
                                       ecs::Query<const Cell>              cells)   //
{                                                                      //
    cells.for_each([window](const Cell& cell) { window->draw(cell.shape); });
};

}   // namespace game
