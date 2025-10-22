module;

#include <SFML/Graphics.hpp>

module snake.game.tasks.draw;

import ddge.modules.ecs;

import ddge.modules.execution.accessors.ecs.Query;
import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.ECSProvider;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.game.Cell;
import snake.window.Window;

using namespace ddge::exec::accessors;

auto game::draw()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        +[](Resource<window::Window> window, Query<const Cell>& cells) -> void {
            cells.for_each([window](const Cell& cell) -> void {
                window->draw(cell.shape);
            });
        }
    );
}
