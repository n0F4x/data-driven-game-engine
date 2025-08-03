module;

#include <SFML/Graphics.hpp>

export module snake.game.draw;

import core.ecs;

import extensions.scheduler.accessors.ecs.Query;
import extensions.scheduler.accessors.resources;

import snake.game.Cell;
import snake.window.Window;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export auto draw(resources::Resource<window::Window> window, ecs::Query<const Cell> cells)
    -> void;

}   // namespace game

module :private;

auto game::draw(resources::Resource<window::Window> window, ecs::Query<const Cell> cells)
    -> void
{
    cells.for_each([window](const Cell& cell) -> void { window->draw(cell.shape); });
}
