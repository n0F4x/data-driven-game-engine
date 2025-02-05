module;

#include <utility>

module ecs;

auto ecs::SystemBuilder::build(core::store::Store& store, Registry& registry) && -> System
{
    return std::move(m_build)(store, registry);
}
