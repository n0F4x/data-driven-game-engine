export module prelude.plugins;

import modules.app.extensions.AddonsPlugin;
import modules.app.extensions.FunctionalPlugin;
import modules.app.extensions.RunnablePlugin;
import modules.assets.Plugin;
import modules.ecs;
import modules.events.Plugin;
import modules.messages.Plugin;
import modules.resources.Plugin;
import modules.scheduler.Plugin;
import modules.states.Plugin;

namespace plugins {

export using Addons     = modules::app::extensions::AddonsPlugin;
export using Assets     = modules::assets::Plugin;
export using ECS        = modules::ecs::Plugin;
export using Events     = modules::events::Plugin;
export using Functional = modules::app::extensions::FunctionalPlugin;
export using Messages   = modules::messages::Plugin;
export using Resources  = modules::resources::Plugin;
export using Runnable   = modules::app::extensions::RunnablePlugin;
export using Scheduler  = modules::scheduler::Plugin;
export using States     = modules::states::Plugin;

}   // namespace plugins
