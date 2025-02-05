module;

#include <entt/entity/registry.hpp>

export module ecs:RegistryAddon;

import :Registry;

namespace ecs {

export struct RegistryAddon {
    Registry registry;
};

}   // namespace ecs
