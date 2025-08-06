module;

#include <utility>

export module snake.game.setup;

import app;

import core.time.FixedTimer;

import plugins.events;
import plugins.messages;
import plugins.functional;
import plugins.states;

import snake.assets.inject_loaders;
import snake.game.AppleDigested;
import snake.game.AppleSpawnTimer;
import snake.game.DigestedApple;
import snake.game.GameOver;
import snake.game.GameState;
import snake.game.Settings;
import snake.game.WorldUpdate;

namespace game {

inline constexpr Settings settings{
    .cells_per_row    = 20,
    .cells_per_column = 20,
    .cell_width       = 32,
};

export inline constexpr auto setup =
    []<app::decays_to_builder_c Builder_T>(Builder_T&& builder)   //
{
    static_assert(app::has_plugins_c<Builder_T, plugins::StatesTag>);
    static_assert(app::has_plugins_c<Builder_T, plugins::Events>);
    static_assert(app::has_plugins_c<Builder_T, plugins::Messages>);
    static_assert(app::has_plugins_c<Builder_T, plugins::Functional>);

    return std::forward<Builder_T>(builder)
        .insert_resource(settings)
        .insert_resource(AppleSpawnTimer{})
        .template register_state<GameState>()
        .template register_event<AppleDigested>()
        .template register_event<GameOver>()
        .template register_message<DigestedApple>()
        .template register_message<WorldUpdate>()
        .transform(assets::inject_loaders);
};

}   // namespace game
