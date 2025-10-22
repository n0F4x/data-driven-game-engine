module;

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module snake.app.tasks.shut_down;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.primitives.start_as;
import ddge.modules.execution.v2.TaskBlueprint;

import snake.game.tasks.shut_down;
import snake.window.tasks.close_window;

namespace app {

export [[nodiscard]]
auto shut_down()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::start_as(game::shut_down())   //
        .then(window::close_window());
}

}   // namespace app
