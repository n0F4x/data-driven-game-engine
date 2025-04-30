export module addons.ecs;

import core.ecs;

namespace addons {

export struct ECS {
    core::ecs::Registry registry;
};

export constexpr inline auto make_ecs = [] static { return ECS{}; };

}   // namespace addons
