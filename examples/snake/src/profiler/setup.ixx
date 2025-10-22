module;

#include <utility>

export module snake.profiler.setup;

import ddge.modules.app.decays_to_builder_c;
import ddge.modules.app.has_plugins_c;
import ddge.modules.resources.Plugin;

import snake.profiler.SecondTimer;

namespace profiler {

export inline constexpr auto setup =
    []<ddge::app::decays_to_builder_c Builder_T>(Builder_T&& builder) -> Builder_T   //
{
    static_assert(ddge::app::has_plugins_c<Builder_T, ddge::resources::Plugin>);

    return std::forward<Builder_T>(builder).insert_resource(SecondTimer{});
};

}   // namespace profiler
