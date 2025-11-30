module;

#include <utility>

export module snake.app.run;

import ddge.modules.app.has_plugins_c;
import ddge.modules.scheduler.Plugin;
import ddge.modules.scheduler.primitives.start_as;

import snake.app.tasks.initialize;
import snake.app.tasks.run_game_loop;
import snake.app.tasks.shut_down;

namespace app {

export template <ddge::app::has_plugins_c<ddge::scheduler::Plugin> AppBuilder_T>
auto run(AppBuilder_T&& app_builder)
{
    return std::forward<AppBuilder_T>(app_builder)
        .run(
            ddge::scheduler::start_as(tasks::initialize())   //
                .then(tasks::run_game_loop())
                .then(tasks::shut_down())
        );
}

}   // namespace app
