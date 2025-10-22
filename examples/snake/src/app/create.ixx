export module snake.app.create;

import ddge.modules.app.create;
import ddge.modules.app.extensions.FunctionalPlugin;

import ddge.modules.assets.Plugin;
import ddge.modules.ecs;
import ddge.modules.events.Plugin;
import ddge.modules.exec.Plugin;
import ddge.modules.messages.Plugin;
import ddge.modules.resources.Plugin;
import ddge.modules.states.Plugin;

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
        .plug_in(ddge::exec::Plugin{})
        .plug_in(ddge::messages::Plugin{})
        .plug_in(ddge::resources::Plugin{})
        .plug_in(ddge::states::Plugin{})
        .transform(setup);
}

}   // namespace app
