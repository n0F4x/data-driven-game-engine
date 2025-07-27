module;

#include <SFML/Graphics.hpp>

export module snake.game.color_cells;

import core.ecs;

import extensions.scheduler.accessors.ecs.Query;

import utility.containers.OptionalRef;

import snake.game.Apple;
import snake.game.Cell;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export auto color_cells(
    ecs::Query<Optional<const SnakeHead>, Optional<const Snake>, Optional<const Apple>, Cell>
        cells
) -> void;

}   // namespace game

module :private;

auto game::color_cells(
    ecs::Query<Optional<const SnakeHead>, Optional<const Snake>, Optional<const Apple>, Cell>
        cells
) -> void
{
    cells.for_each(   //
        [](const util::OptionalRef<const SnakeHead> snake_head,
           const util::OptionalRef<const Snake>     snake_body,
           const util::OptionalRef<const Apple>     apple,
           Cell&                                    cell)   //
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
