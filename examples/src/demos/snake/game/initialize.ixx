module;

#include <cassert>
#include <cstdint>
#include <random>
#include <ranges>

#include <SFML/Graphics.hpp>

export module snake.game.initialize;

import core.ecs;
import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.assets.Cached;
import extensions.scheduler.accessors.ecs.Registry;
import extensions.scheduler.accessors.resources.Resource;


import snake.assets.load_texture;
import snake.game.Cell;
import snake.game.color_cells;
import snake.game.Direction;
import snake.game.game_tick_rate;
import snake.game.GameOver;
import snake.game.Position;
import snake.game.Settings;
import snake.game.Snake;
import snake.game.SnakeHead;
import snake.window.Window;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

using TextureLoader = extensions::scheduler::accessors::assets::
    Cached<std::remove_cvref_t<decltype(::assets::load_texture)>>;

namespace game {

auto initialize_map(
    resources::Resource<const window::Window> window,
    resources::Resource<const Settings>       settings,
    ecs::Registry                             registry
) -> void;

auto initialize_snake(resources::Resource<const Settings> settings, ecs::Registry registry)
    -> void;

auto load_apple_texture(TextureLoader texture_loader) -> void;

auto reset_timer(resources::Resource<core::time::FixedTimer<game_tick_rate>> game_timer)
    -> void;

export inline constexpr auto initialize =   //
    core::scheduler::start_as(initialize_map)
        .then(initialize_snake)
        .then(color_cells)
        .then(reset_timer);

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

auto game::initialize_map(
    const resources::Resource<const window::Window> window,
    const resources::Resource<const Settings>       settings,
    const ecs::Registry                             registry
) -> void
{
    assert(settings->cell_width * settings->cells_per_row <= window->getSize().x);
    assert(settings->cell_width * settings->cells_per_column <= window->getSize().y);

    const uint16_t starting_pixel_x{ static_cast<uint16_t>(
        (window->getSize().x - settings->cell_width * settings->cells_per_row) / 2
    ) };
    const uint16_t starting_pixel_y{ static_cast<uint16_t>(
        (window->getSize().y - settings->cell_width * settings->cells_per_column) / 2
    ) };

    // TODO: use std::views::cartesian_product
    for (const uint8_t row_index : std::views::iota(uint8_t{}, settings->cells_per_row)) {
        for (const uint8_t column_index :
             std::views::iota(uint8_t{}, settings->cells_per_column))
        {
            registry->create(
                Cell{},
                Position{ .row = row_index, .column = column_index },
                make_shape(
                    starting_pixel_x + settings->cell_width * row_index,
                    starting_pixel_y + settings->cell_width * column_index,
                    settings->cell_width
                )
            );
        }
    }
}

auto game::initialize_snake(
    const resources::Resource<const Settings> settings,
    const ecs::Registry                       registry
) -> void
{
    std::random_device            random_device;
    std::mt19937                  random_engine{ random_device() };
    std::uniform_int_distribution row_distribution{
        uint8_t{}, static_cast<uint8_t>(settings->cells_per_row - 1)
    };
    std::uniform_int_distribution column_distribution{
        uint8_t{}, static_cast<uint8_t>(settings->cells_per_column - 1)
    };

    const Position snake_position{
        .row    = row_distribution(random_engine),
        .column = column_distribution(random_engine),
    };

    const Direction snake_direction{ [snake_position, &settings] {
        if (snake_position.row < settings->cells_per_row / 2) {
            return Direction::eRight;
        }
        return Direction::eLeft;
    }() };

    std::optional<core::ecs::ID> snake_head_id;
    core::ecs::query(
        registry.get(),
        [&snake_head_id,
         snake_position](const core::ecs::ID id, With<Cell>, const Position position) {
            if (position == snake_position) {
                assert(!snake_head_id.has_value());
                snake_head_id = id;
            }
        }
    );
    assert(snake_head_id.has_value());

    registry->insert(
        *snake_head_id, Snake{ .charge = 1 }, SnakeHead{ .direction = snake_direction }
    );
}

auto game::reset_timer(
    resources::Resource<core::time::FixedTimer<game_tick_rate>> game_timer
) -> void
{
    game_timer->reset();
}
