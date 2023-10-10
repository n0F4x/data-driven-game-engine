#pragma once

#include "engine/app/Plugin.hpp"
#include "engine/utility/Context.hpp"

namespace engine {

namespace resource_manager {

using ResourceManager = utils::Context;

}   // namespace resource_manager

namespace plugins {

class ResourceManager {
public:
    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Context& t_context) noexcept -> void;
};

static_assert(PluginConcept<ResourceManager>);

}   // namespace plugins

}   // namespace engine
