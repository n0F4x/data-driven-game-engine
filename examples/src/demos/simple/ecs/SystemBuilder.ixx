module;

#include <functional>

#include <entt/entity/fwd.hpp>

export module ecs:SystemBuilder;

import core.store.Store;

import :Registry;
import :schedulable_c;
import :System;

namespace ecs {

export class SystemBuilder {
public:
    template <ecs::schedulable_c Task_T>
    explicit SystemBuilder(Task_T&& task);

    [[nodiscard]]
    auto build(core::store::Store& store, Registry& registry) && -> System;

private:
    std::function<ecs::System(core::store::Store&, Registry&)> m_build;
};

}   // namespace ecs

template <ecs::schedulable_c Task_T>
ecs::SystemBuilder::SystemBuilder(Task_T&& task)
    : m_build{ [task = std::forward<Task_T>(task
                )](core::store::Store& store, Registry& registry) {
          return ecs::System{ std::move(task), store, registry };
      } }
{}
