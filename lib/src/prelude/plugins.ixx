export module ddge.prelude.plugins;

export import ddge.modules.app.extensions.AddonsPlugin;
export import ddge.modules.app.extensions.FunctionalPlugin;
export import ddge.modules.app.extensions.RunnablePlugin;
export import ddge.modules.assets.Plugin;
import ddge.modules.ecs;
export import ddge.modules.events.Plugin;
export import ddge.modules.messages.Plugin;
export import ddge.modules.resources.Plugin;
export import ddge.modules.scheduler.Plugin;
export import ddge.modules.states.Plugin;

namespace ddge::ecs {

export using ecs::Plugin;

}   // namespace ddge::ecs
