module;

#include <utility>

export module snake.game.plugin;

import app;

import core.time.FixedTimer;

import plugins.events;

import snake.game.game_tick_rate;
import snake.game.GameOver;
import snake.game.Settings;

namespace game {

export constexpr inline auto make_plugin = [](Settings settings) {
    return [settings = std::move(settings)]<app::decays_to_builder_c Builder_T>(
               Builder_T&& builder
           )   //
    {
        static_assert(app::has_plugins_c<Builder_T, plugins::EventsTag>);

        return std::forward<Builder_T>(builder)
            .use_resource(settings)
            .use_resource(core::time::FixedTimer<game_tick_rate>{})
            .template register_event<GameOver>();
    };
};

}   // namespace game
