module;

#include <utility>

export module snake.game.plugin;

import app;

import core.time.FixedTimer;

import plugins.events;
import plugins.functional;

import snake.assets.inject_loaders;
import snake.game.game_tick_rate;
import snake.game.GameOver;
import snake.game.Settings;

namespace game {

inline constexpr Settings settings{
    .cells_per_row    = 20,
    .cells_per_column = 20,
    .cell_width       = 32,
};

export inline constexpr auto setup =
    []<app::decays_to_builder_c Builder_T>(Builder_T&& builder)   //
{
    static_assert(app::has_plugins_c<Builder_T, plugins::EventsTag>);
    static_assert(app::has_plugins_c<Builder_T, plugins::Functional>);

    return std::forward<Builder_T>(builder)
        .insert_resource(settings)
        .insert_resource(core::time::FixedTimer<game_tick_rate>{})
        .template register_event<GameOver>()
        .transform(assets::inject_loaders);
};

}   // namespace game
