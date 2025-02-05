module;

#include <algorithm>
#include <ranges>
#include <vector>

module ecs;

import :Registry;
import :System;

auto ecs::Scheduler::run(core::store::Store& store, Registry& registry) && -> void
{
    std::ranges::for_each(
        m_system_builders | std::views::as_rvalue
            | std::views::transform([&store, &registry](SystemBuilder&& system_builder) {
                  return std::move(system_builder).build(store, registry);
              }),
        &System::operator()
    );
}
