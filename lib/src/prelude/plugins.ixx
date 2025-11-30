export module ddge.prelude.plugins;

import ddge.modules.app.extensions.AddonsPlugin;
import ddge.modules.app.extensions.FunctionalPlugin;
import ddge.modules.app.extensions.RunnablePlugin;
import ddge.modules.assets.Plugin;
import ddge.modules.ecs;
import ddge.modules.events.Plugin;
import ddge.modules.messages.Plugin;
import ddge.modules.resources.Plugin;
import ddge.modules.scheduler.Plugin;
import ddge.modules.states.Plugin;

namespace ddge::plugins {

export using Addons     = app::extensions::AddonsPlugin;
export using Functional = app::extensions::FunctionalPlugin;
export using Runnable   = app::extensions::RunnablePlugin;

export using Assets    = assets::Plugin;
export using ECS       = ecs::Plugin;
export using Events    = events::Plugin;
export using Scheduler = scheduler::Plugin;
export using Messages  = messages::Plugin;
export using Resources = resources::Plugin;
export using States    = states::Plugin;

}   // namespace ddge::plugins
