module;

#include <SFML/Graphics.hpp>

export module snake.game.color_cells;

import ddge.modules.ecs;

import ddge.modules.execution.accessors.ecs.Query;

import ddge.utility.containers.OptionalRef;

import snake.game.Apple;
import snake.game.Cell;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace ddge::exec::accessors;
using namespace ddge::ecs::query_parameter_tags;

namespace game {

export auto color_cells(
    Query<Optional<const SnakeHead>, Optional<const Snake>, Optional<const Apple>, Cell>&
        cells
) -> void;

}   // namespace game

module :private;

auto game::color_cells(
    Query<Optional<const SnakeHead>, Optional<const Snake>, Optional<const Apple>, Cell>&
        cells
) -> void
{
    cells.for_each(   //
        [](const ddge::util::OptionalRef<const SnakeHead> snake_head,
           const ddge::util::OptionalRef<const Snake>     snake_body,
           const ddge::util::OptionalRef<const Apple>     apple,
           Cell&                                          cell)   //
        {
            if (snake_head.has_value()) {
                cell.shape.setFillColor(sf::Color{ 76, 187, 23 });
            }
            else if (snake_body.has_value()) {
                cell.shape.setFillColor(sf::Color{ 0, 128, 0 });
            }
            else if (apple.has_value()) {
                cell.shape.setFillColor(sf::Color::White);
            }
            else {
                cell.shape.setFillColor(sf::Color::Black);
            }
        }
    );
}
