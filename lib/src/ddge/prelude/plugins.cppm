export module ddge.prelude.plugins;

export import ddge.app.extensions.AddonsPlugin;
export import ddge.app.extensions.FunctionalPlugin;
export import ddge.app.extensions.RunnablePlugin;
export import ddge.assets.Plugin;
import ddge.ecs;
export import ddge.events.Plugin;
export import ddge.messages.Plugin;
export import ddge.resources.Plugin;
export import ddge.scheduler.Plugin;
export import ddge.states.Plugin;

namespace ddge::ecs {

export using ecs::Plugin;

}   // namespace ddge::ecs
