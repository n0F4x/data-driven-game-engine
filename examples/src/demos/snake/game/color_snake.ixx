module;

#include <optional>

#include <SFML/Graphics.hpp>

export module game.color_snake;

import core.ecs;

import extensions.scheduler.accessors.ecs.Query;

import game.Snake;
import game.SnakeHead;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export constexpr inline auto color_snake =
    [](   //
        ecs::Query<With<Snake>, Optional<const SnakeHead>, sf::RectangleShape> charged_cells
    ) {
        charged_cells.for_each(   //
            [](const std::optional<SnakeHead> snake_head, sf::RectangleShape& shape) {
                if (snake_head.has_value()) {
                    shape.setFillColor(sf::Color{ 76, 187, 23 });
                }
                else {
                    shape.setFillColor(sf::Color{ 0, 128, 0 });
                }
            }
        );
    };

}   // namespace game
