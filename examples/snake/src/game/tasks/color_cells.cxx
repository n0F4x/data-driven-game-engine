module;

#include <SFML/Graphics.hpp>

module snake.game.tasks.color_cells;

import ddge.modules.ecs;

import ddge.modules.scheduler.accessors.ecs;
import ddge.modules.scheduler.primitives.query;

import ddge.utility.containers.OptionalRef;

import snake.game.Apple;
import snake.game.Cell;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace ddge::scheduler::accessors;
using namespace ddge::ecs::query_filter_tags;

auto game::tasks::color_cells() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::query(
        [](const Optional<const SnakeHead> snake_head,
           const Optional<const Snake>     snake_body,
           const Optional<const Apple>     apple,
           Cell&                           cell) static -> void   //
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
