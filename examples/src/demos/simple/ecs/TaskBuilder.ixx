module;

#include <functional>

#include <entt/entity/fwd.hpp>

export module ecs:TaskBuilder;

import core.resource.ResourceManager;
import utility.meta.type_traits.functional.arguments_of;

import :Registry;
import :system_c;
import :Task;

namespace ecs {

export template <typename... Params_T>
class TaskBuilder {
public:
    template <system_c System_T>
    explicit TaskBuilder(System_T&& system);

    template <typename... Resources_T>
    [[nodiscard]]
    auto build(
        core::resource::ResourceManager<Resources_T...>& resources,
        Registry&                                        registry
    ) && -> Task;

private:
    std::function<void(Params_T...)> m_system;
};

}   // namespace ecs

template <typename... Params_T>
template <ecs::system_c System_T>
ecs::TaskBuilder<Params_T...>::TaskBuilder(System_T&& system)
    : m_system{ std::move(system) }
{}

template <typename... Params_T>
template <typename... Resources_T>
auto ecs::TaskBuilder<Params_T...>::build(
    core::resource::ResourceManager<Resources_T...>& resources,
    ecs::Registry&                                   registry
) && -> Task
{
    return Task{ std::move(m_system), resources, registry };
}
