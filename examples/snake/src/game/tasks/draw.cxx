module;

#include <SFML/Graphics.hpp>

module snake.game.tasks.draw;

import ddge.ecs;

import ddge.scheduler.accessors.ecs;
import ddge.scheduler.accessors.resources;
import ddge.scheduler.primitives.force_on_main;

import snake.game.Cell;
import snake.window.Window;

using namespace ddge::scheduler::accessors;

auto game::tasks::draw() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::force_on_main(
        +[](Resource<window::Window> window, Query<const Cell>& cells) -> void {
            cells.for_each([window](const Cell& cell) -> void {
                window->draw(cell.shape);
            });
        }
    );
}
