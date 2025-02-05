module;

#include <functional>
#include <vector>

export module ecs:Scheduler;

import :Registry;
import :schedulable_c;
import :SystemBuilder;

import core.store;

namespace ecs {

export class Scheduler {
public:
    template <typename Self, schedulable_c Schedulable_T>
    auto schedule(this Self&&, Schedulable_T&& schedulable) -> Self;

    auto run(core::store::Store& resources, Registry& registry) && -> void;

private:
    std::vector<SystemBuilder> m_system_builders;
};

}   // namespace ecs

template <typename Self, ecs::schedulable_c Schedulable_T>
auto ecs::Scheduler::schedule(this Self&& self, Schedulable_T&& schedulable) -> Self
{
    self.m_system_builders.emplace_back(std::forward<Schedulable_T>(schedulable));

    return std::forward<Self>(self);
}
