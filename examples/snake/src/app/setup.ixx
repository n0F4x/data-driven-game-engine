module;

#include <utility>

export module snake.app.setup;

import ddge.modules.app.decays_to_builder_c;
import ddge.modules.app.has_plugins_c;
import ddge.modules.app.extensions.FunctionalPlugin;

import snake.game.setup;
import snake.profiler.setup;
import snake.window.setup;

namespace app {

export inline constexpr auto setup =
    []<ddge::app::decays_to_builder_c Builder_T>(Builder_T&& builder) -> Builder_T   //
{
    static_assert(ddge::app::has_plugins_c<Builder_T, ddge::app::FunctionalPlugin>);

    return std::forward<Builder_T>(builder)
        .transform(window::setup)
        .transform(game::setup)
        .transform(profiler::setup);
};

}   // namespace app
