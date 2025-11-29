module;

#include <utility>

export module snake.game.setup;

import ddge.modules.app.decays_to_builder_c;
import ddge.modules.app.extensions.FunctionalPlugin;
import ddge.modules.app.has_plugins_c;
import ddge.modules.events.Plugin;
import ddge.modules.messages.Plugin;
import ddge.modules.states.Plugin;
import ddge.modules.time.FixedTimer;

import snake.assets.inject_loaders;
import snake.game.AppleDigested;
import snake.game.AppleSpawnTimer;
import snake.game.DigestedApple;
import snake.game.GameOver;
import snake.game.GameState;
import snake.game.Settings;
import snake.game.WorldUpdate;

namespace game {

export inline constexpr auto setup =
    []<ddge::app::decays_to_builder_c Builder_T>(Builder_T&& builder) -> Builder_T   //
{
    constexpr Settings settings{
        .cells_per_row    = 20,
        .cells_per_column = 20,
        .cell_width       = 32,
    };

    static_assert(ddge::app::has_plugins_c<Builder_T, ddge::states::Plugin>);
    static_assert(ddge::app::has_plugins_c<Builder_T, ddge::events::Plugin>);
    static_assert(ddge::app::has_plugins_c<Builder_T, ddge::messages::Plugin>);
    static_assert(
        ddge::app::has_plugins_c<Builder_T, ddge::app::extensions::FunctionalPlugin>
    );

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
