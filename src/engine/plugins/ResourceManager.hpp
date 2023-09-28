#pragma once

#include <entt/entity/registry.hpp>

#include "engine/app/Plugin.hpp"

namespace engine {

namespace resource_manager {

using ResourceManager = entt::registry::context;

}   // namespace resource_manager

namespace plugins {

class ResourceManager {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto setup(App::Context& t_context) const noexcept -> void;
};

static_assert(PluginConcept<ResourceManager>);

}   // namespace plugins

}   // namespace engine
