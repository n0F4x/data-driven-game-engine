module;

#include <SFML/Graphics.hpp>

module snake.game.tasks.draw;

import ddge.modules.ecs;

import ddge.modules.scheduler.accessors.ecs;
import ddge.modules.scheduler.accessors.resources;
import ddge.modules.scheduler.primitives.force_on_main;

import snake.game.Cell;
import snake.window.Window;

using namespace ddge::scheduler::accessors;

auto game::tasks::draw()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>
{
    return ddge::scheduler::force_on_main(
        +[](Resource<window::Window> window, Query<const Cell>& cells) -> void {
            cells.for_each([window](const Cell& cell) -> void {
                window->draw(cell.shape);
            });
        }
    );
}
