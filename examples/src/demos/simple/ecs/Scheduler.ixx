module;

#include <algorithm>
#include <functional>
#include <ranges>
#include <tuple>

export module ecs:Scheduler;

import :Registry;
import :SchedulerAddonTag;
import :system_c;
import :TaskBuilder;

import core.resource.ResourceManager;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.type_list.type_list_to;

template <util::meta::specialization_of_c<ecs::TaskBuilder>... TaskBuilders_T>
class Scheduler;

namespace ecs {

export using Scheduler = ::Scheduler<>;

export template <util::meta::specialization_of_c<::Scheduler> Scheduler_T>
struct SchedulerAddon;

}   // namespace ecs

template <typename System_T>
using task_builder_for = util::meta::type_list_to_t<
    util::meta::arguments_of_t<System_T>,
    ecs::TaskBuilder>;

template <util::meta::specialization_of_c<ecs::TaskBuilder>... TaskBuilders_T>
class Scheduler {
public:
    using Addon    = ecs::SchedulerAddon<Scheduler>;
    using AddonTag = ecs::SchedulerAddonTag;

    Scheduler()
        requires(sizeof...(TaskBuilders_T) == 0)
    = default;

    template <typename Self_T, ecs::system_c System_T>
    auto schedule(this Self_T&&, System_T&& system)
        -> Scheduler<TaskBuilders_T..., task_builder_for<System_T>>;

    template <typename... Resources_T>
    auto run(
        core::resource::ResourceManager<Resources_T...>& resources,
        ecs::Registry&                                   registry
    ) && -> void;

private:
    template <util::meta::specialization_of_c<ecs::TaskBuilder>...>
    friend class Scheduler;

    std::tuple<TaskBuilders_T...> m_task_builders;

    explicit Scheduler(const std::tuple<TaskBuilders_T...>& task_builders);
    explicit Scheduler(std::tuple<TaskBuilders_T...>&& task_builders);
};

template <util::meta::specialization_of_c<ecs::TaskBuilder>... TaskBuilders_T>
template <typename Self_T, ecs::system_c System_T>
auto Scheduler<TaskBuilders_T...>::schedule(this Self_T&& self, System_T&& system)
    -> Scheduler<TaskBuilders_T..., task_builder_for<System_T>>
{
    return Scheduler<TaskBuilders_T..., task_builder_for<System_T>>{ std::tuple_cat(
        std::forward_like<Self_T>(self.m_task_builders),
        std::make_tuple(task_builder_for<System_T>{ std::forward<System_T>(system) })
    ) };
}

template <util::meta::specialization_of_c<ecs::TaskBuilder>... TaskBuilders_T>
template <typename... Resources_T>
auto Scheduler<TaskBuilders_T...>::run(
    core::resource::ResourceManager<Resources_T...>& resources,
    ecs::Registry&                                   registry
) && -> void
{
    std::apply(
        [resources = std::ref(resources),
         &registry]<typename TaskBuilder_T>(TaskBuilder_T&& task_builder) {
            std::invoke(
                std::forward<TaskBuilder_T>(task_builder).build(resources.get(), registry)
            );
        },
        std::move(m_task_builders)
    );
}

template <util::meta::specialization_of_c<ecs::TaskBuilder>... TaskBuilders_T>
Scheduler<TaskBuilders_T...>::Scheduler(const std::tuple<TaskBuilders_T...>& task_builders)
    : m_task_builders{ task_builders }
{}

template <util::meta::specialization_of_c<ecs::TaskBuilder>... TaskBuilders_T>
Scheduler<TaskBuilders_T...>::Scheduler(std::tuple<TaskBuilders_T...>&& task_builders)
    : m_task_builders{ std::move(task_builders) }
{}
