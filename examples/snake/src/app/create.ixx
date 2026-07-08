export module snake.app.create;

import ddge.app.create;
import ddge.app.extensions.FunctionalPlugin;

import ddge.assets.Plugin;
import ddge.ecs;
import ddge.events.Plugin;
import ddge.scheduler.Plugin;
import ddge.messages.Plugin;
import ddge.resources.Plugin;
import ddge.states.Plugin;

import snake.app.setup;

namespace app {

export [[nodiscard]]
auto create()
{
    return ddge::app::create()
        .plug_in(ddge::app::FunctionalPlugin{})
        .plug_in(ddge::assets::Plugin{})
        .plug_in(ddge::ecs::Plugin{})
        .plug_in(ddge::events::Plugin{})
        .plug_in(ddge::scheduler::Plugin{ 2 })
        .plug_in(ddge::messages::Plugin{})
        .plug_in(ddge::resources::Plugin{})
        .plug_in(ddge::states::Plugin{})
        .transform(setup);
}

}   // namespace app
