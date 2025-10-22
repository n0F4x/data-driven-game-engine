module;

#include <utility>

export module snake.app.run;

import ddge.modules.app.has_plugins_c;
import ddge.modules.execution.Plugin;
import ddge.modules.execution.providers;
import ddge.modules.execution.v2.primitives.start_as;

import snake.app.tasks.initialize;
import snake.app.tasks.run_game_loop;
import snake.app.tasks.shut_down;

namespace app {

export template <ddge::app::has_plugins_c<ddge::exec::Plugin> AppBuilder_T>
auto run(AppBuilder_T&& app_builder)
{
    return std::forward<AppBuilder_T>(app_builder)
        .run(
            ddge::exec::v2::start_as(initialize())   //
                .then(run_game_loop())
                .then(shut_down())
        );
}

}   // namespace app
