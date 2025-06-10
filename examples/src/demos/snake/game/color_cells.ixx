module;

#include <SFML/Graphics.hpp>

export module snake.game.color_cells;

import core.ecs;

import extensions.scheduler.accessors.ecs.Query;

import utility.containers.OptionalRef;

import snake.game.Cell;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export constexpr inline auto color_cells =   //
    [](ecs::Query<
        With<Cell>,
        Optional<const SnakeHead>,
        Optional<const Snake>,
        sf::RectangleShape> cells)   //
{
    cells.for_each(                  //
        [](const util::OptionalRef<const SnakeHead> snake_head,
           const util::OptionalRef<const Snake>     snake_body,
           sf::RectangleShape&                      shape)   //
        {
            if (snake_head.has_value()) {
                shape.setFillColor(sf::Color{ 76, 187, 23 });
            }
            else if (snake_body.has_value()) {
                shape.setFillColor(sf::Color{ 0, 128, 0 });
            }
            else {
                shape.setFillColor(sf::Color::Black);
            }
        }
    );
};

}   // namespace game
