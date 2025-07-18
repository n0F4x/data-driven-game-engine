module;

#include <cassert>
#include <optional>
#include <utility>
#include <vector>

#include <SFML/Window/Keyboard.hpp>

export module snake.game.move_snake;

import core.ecs;

import extensions.scheduler.accessors.ecs.Registry;
import extensions.scheduler.accessors.events.Recorder;
import extensions.scheduler.accessors.resources;

import snake.game.Cell;
import snake.game.Direction;
import snake.game.GameOver;
import snake.game.Position;
import snake.game.Settings;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace extensions::scheduler::accessors;
using namespace core::ecs::query_parameter_tags;

namespace game {

auto correct_snake_direction(core::ecs::Registry& registry) -> void;

auto move_snake_head(
    const events::Recorder<GameOver>& game_over_recorder,
    const Settings&                   settings,
    core::ecs::Registry&              registry
) -> void;

auto decrease_charges(core::ecs::Registry& registry) -> void;

export inline constexpr auto move_snake =
    [](const resources::Resource<const Settings> settings,
       const events::Recorder<GameOver>&         game_over_recorder,
       const ecs::Registry                       registry)   //
{
    correct_snake_direction(registry.get());
    move_snake_head(game_over_recorder, settings.get(), registry.get());
    decrease_charges(registry.get());
};

}   // namespace game

[[nodiscard]]
auto direction_mixed_with_user_input(game::Direction direction) -> game::Direction
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        switch (direction) {
            case game::Direction::eRight: direction = game::Direction::eUp; break;
            case game::Direction::eDown:  direction = game::Direction::eRight; break;
            case game::Direction::eLeft:  direction = game::Direction::eDown; break;
            case game::Direction::eUp:    direction = game::Direction::eLeft; break;
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        switch (direction) {
            case game::Direction::eRight: direction = game::Direction::eDown; break;
            case game::Direction::eDown:  direction = game::Direction::eLeft; break;
            case game::Direction::eLeft:  direction = game::Direction::eUp; break;
            case game::Direction::eUp:    direction = game::Direction::eRight; break;
        }
    }
    return direction;
}

auto game::correct_snake_direction(core::ecs::Registry& registry) -> void
{
    core::ecs::query(registry, [](SnakeHead& snake_head) {
        snake_head.direction = direction_mixed_with_user_input(snake_head.direction);
    });
}

[[nodiscard]]
auto next_position(
    const game::Settings& settings,
    const game::Position  position,
    const game::Direction direction
) -> std::optional<game::Position>
{
    switch (direction) {
        case game::Direction::eRight: {
            if (position.row == settings.cells_per_row - 1) {
                return std::nullopt;
            }

            return game::Position{
                .row    = static_cast<uint8_t>(position.row + 1),
                .column = position.column,
            };
        }
        case game::Direction::eDown: {
            if (position.column == settings.cells_per_column - 1) {
                return std::nullopt;
            }

            return game::Position{
                .row    = position.row,
                .column = static_cast<uint8_t>(position.column + 1),
            };
        }
        case game::Direction::eLeft: {
            if (position.row == 0) {
                return std::nullopt;
            }

            return game::Position{
                .row    = static_cast<uint8_t>(position.row - 1),
                .column = position.column,
            };
        }
        case game::Direction::eUp: {
            if (position.column == 0) {
                return std::nullopt;
            }

            return game::Position{
                .row    = position.row,
                .column = static_cast<uint8_t>(position.column - 1),
            };
        }
    }
    std::unreachable();
}

auto game::move_snake_head(
    const events::Recorder<GameOver>& game_over_recorder,
    const Settings&                   settings,
    core::ecs::Registry&              registry
) -> void
{
    std::optional<core::ecs::ID> snake_head_id;
    core::ecs::query(registry, [&snake_head_id](const core::ecs::ID id, With<SnakeHead>) {
        assert(!snake_head_id.has_value());
        snake_head_id = id;
    });
    assert(snake_head_id.has_value());

    const auto snake_head{ registry.remove_single<SnakeHead>(*snake_head_id) };
    const auto [cell, snake_body]{ registry.get<Cell, Snake>(*snake_head_id) };
    assert((!registry.contains_all<SnakeHead>(*snake_head_id)));

    const std::optional<Position> next_pos{
        next_position(settings, cell.position, snake_head.direction)
    };
    if (!next_pos.has_value()) {
        game_over_recorder.record();
        return;
    }

    std::optional<core::ecs::ID> new_snake_head_id;
    core::ecs::query(
        registry,
        [&new_snake_head_id,
         &next_pos](const core::ecs::ID id, const Cell& new_cell) {
            if (new_cell.position == *next_pos) {
                assert(!new_snake_head_id.has_value());
                new_snake_head_id = id;
            }
        }
    );
    assert(new_snake_head_id.has_value());

    if (registry.contains_all<Snake>(*new_snake_head_id)) {
        game_over_recorder.record();
        return;
    }

    registry.insert(*new_snake_head_id, snake_head, snake_body);
}

auto game::decrease_charges(core::ecs::Registry& registry) -> void
{
    std::vector<core::ecs::ID> lost_cells;

    core::ecs::query(
        registry,
        [&lost_cells](const core::ecs::ID id, Snake& snake_body, Without<SnakeHead>) {
            snake_body.charge--;
            if (snake_body.charge == 0) {
                lost_cells.push_back(id);
            }
        }
    );

    for (const core::ecs::ID id : lost_cells) {
        registry.remove<Snake>(id);
    }
}
