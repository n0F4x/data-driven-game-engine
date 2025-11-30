module;

#include <SFML/Graphics.hpp>

module snake.game.tasks.draw;

import ddge.modules.ecs;

import ddge.modules.exec.accessors.ecs;
import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.primitives.force_on_main;

import snake.game.Cell;
import snake.window.Window;

using namespace ddge::exec::accessors;

auto game::tasks::draw()
    -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::force_on_main(
        +[](Resource<window::Window> window, Query<const Cell>& cells) -> void {
            cells.for_each([window](const Cell& cell) -> void {
                window->draw(cell.shape);
            });
        }
    );
}
