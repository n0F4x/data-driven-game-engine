module;

#include <SFML/Graphics.hpp>

export module snake.game.draw;

import core.ecs;

import core.scheduler.accessors.ecs.Query;
import core.scheduler.accessors.resources;

import snake.game.Cell;
import snake.window.Window;

using namespace core::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export auto draw(Resource<window::Window> window, Query<const Cell>& cells) -> void;

}   // namespace game

module :private;

auto game::draw(Resource<window::Window> window, Query<const Cell>& cells) -> void
{
    cells.for_each([window](const Cell& cell) -> void { window->draw(cell.shape); });
}
