module;

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module snake.app.tasks.render;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.primitives.at_fixed_rate;
import ddge.modules.execution.v2.primitives.start_as;
import ddge.modules.execution.v2.TaskBlueprint;

import snake.game.tasks.draw;
import snake.window.DisplayTimer;
import snake.window.tasks.clear_window;
import snake.window.tasks.display;

namespace app {

export [[nodiscard]]
auto render() -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::at_fixed_rate<window::DisplayTimer>(   //
        ddge::exec::v2::start_as(window::clear_window())          //
            .then(game::draw())
            .then(window::display())
    );
}

}   // namespace app
