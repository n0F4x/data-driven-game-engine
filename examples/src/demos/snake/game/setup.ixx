module;

#include <cassert>
#include <cstdint>
#include <random>
#include <ranges>

#include <SFML/Graphics.hpp>

export module game.setup;

import core.ecs;

import extensions.scheduler.accessors.ecs.RegistryRef;

import game.Cell;
import game.Direction;
import game.Position;
import game.Snake;
import game.SnakeHead;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

export struct Settings {
    uint16_t window_width;
    uint16_t window_height;
    uint8_t  cells_per_row;
    uint8_t  cells_per_column;
    uint8_t  cell_width;
};

auto setup_map(const Settings& settings, core::ecs::Registry& registry) -> void;

auto setup_snake(const Settings& settings, core::ecs::Registry& registry) -> void;

export inline constexpr auto make_setup = [](Settings settings) {
    return [settings](const ecs::RegistryRef registry) {
        setup_map(settings, registry.get());
        setup_snake(settings, registry.get());
    };
};

}   // namespace game

[[nodiscard]]
auto make_shape(const uint16_t position_x, const uint16_t position_y, const uint8_t width)
    -> sf::RectangleShape
{
    sf::RectangleShape result{
        sf::Vector2f{ static_cast<float>(width), static_cast<float>(width) }
    };
    result.setPosition(
        sf::Vector2f{ static_cast<float>(position_x), static_cast<float>(position_y) }
    );
    result.setFillColor(sf::Color::Black);
    return result;
}

auto game::setup_map(const Settings& settings, core::ecs::Registry& registry) -> void
{
    assert(settings.cell_width * settings.cells_per_row <= settings.window_width);
    assert(settings.cell_width * settings.cells_per_column <= settings.window_height);

    const uint16_t starting_pixel_x{ static_cast<uint16_t>(
        (settings.window_width - settings.cell_width * settings.cells_per_row) / 2
    ) };
    const uint16_t starting_pixel_y{ static_cast<uint16_t>(
        (settings.window_height - settings.cell_width * settings.cells_per_column) / 2
    ) };

    // TODO: use std::views::cartesian_product
    for (const uint8_t row_index : std::views::iota(uint8_t{}, settings.cells_per_row)) {
        for (const uint8_t column_index :
             std::views::iota(uint8_t{}, settings.cells_per_column))
        {
            registry.create(
                Cell{},
                Position{ .row = row_index, .column = column_index },
                make_shape(
                    starting_pixel_x + settings.cell_width * row_index,
                    starting_pixel_y + settings.cell_width * column_index,
                    settings.cell_width
                )
            );
        }
    }
}

auto game::setup_snake(const Settings& settings, core::ecs::Registry& registry) -> void
{
    std::random_device            random_device;
    std::mt19937                  random_engine{ random_device() };
    std::uniform_int_distribution row_distribution{
        uint8_t{}, static_cast<uint8_t>(settings.cells_per_row - 1)
    };
    std::uniform_int_distribution column_distribution{
        uint8_t{}, static_cast<uint8_t>(settings.cells_per_column - 1)
    };

    const Position snake_position{
        .row    = row_distribution(random_engine),
        .column = column_distribution(random_engine),
    };

    const Direction snake_direction{ [snake_position, &settings] {
        if (snake_position.row < settings.cells_per_row / 2) {
            return Direction::eRight;
        }
        return Direction::eLeft;
    }() };

    core::ecs::query(
        registry,
        [&registry,
         snake_position,
         snake_direction](const core::ecs::ID id, With<Cell>, const Position position) {
            if (position == snake_position) {
                registry.insert(
                    id, Snake{ .charge = 1 }, SnakeHead{ .direction = snake_direction }
                );
            }
        }
    );
}
