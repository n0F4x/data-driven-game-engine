module;

#include <utility>

export module ecs:schedule_runner;

import :RegistryAddon;
import :SchedulerAddon;

import addons.ResourceManager;

import core.app.App;

namespace ecs {

export constexpr auto schedule_runner{
    [](core::app::
           has_addons_c<addons::ResourceManager, ecs::SchedulerAddon, ecs::RegistryAddon> auto
               app) -> void { std::move(app.scheduler).run(app.resources, app.registry); }
};

}   // namespace ecs
