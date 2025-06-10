module;

#include <utility>

export module snake.game.plugin;

import core.app;
import core.time.FixedTimer;

import extensions.EventManager;

import snake.game.game_tick_rate;
import snake.game.GameOver;
import snake.game.Settings;

namespace game {

export constexpr inline auto make_plugin = [](Settings settings) {
    return [settings = std::move(settings)]<core::app::decays_to_builder_c Builder_T>(
               Builder_T&& builder
           )   //
    {
        static_assert(core::app::extended_with_c<Builder_T, extensions::EventManagerTag>);

        return std::forward<Builder_T>(builder)
            .use_resource(settings)
            .use_resource(core::time::FixedTimer<game_tick_rate>{})
            .template register_event<GameOver>();
    };
};

}   // namespace game
