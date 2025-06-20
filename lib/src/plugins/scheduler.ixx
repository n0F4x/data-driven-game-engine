module;

#include <functional>
#include <type_traits>
#include <utility>

export module plugins.scheduler;

import app.builder_c;

import core.scheduler;

import extensions.scheduler.argument_provider_c;
import extensions.scheduler.argument_provider_for;

import utility.meta.algorithms.apply;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_transform;

namespace plugins {

export class Scheduler {
public:
    template <app::builder_c Self_T, core::scheduler::decays_to_task_builder_c TaskBuilder_T>
    constexpr auto run(this Self_T&&, TaskBuilder_T&& task_builder);
};

export inline constexpr Scheduler scheduler;

}   // namespace plugins

template <typename App_T>
struct AddonTraits {
    template <typename Addon_T>
        struct HasArgumentProvider : std::bool_constant < requires {
        extensions::scheduler::argument_provider_c<
            extensions::scheduler::argument_provider_for_t<Addon_T>,
            App_T>;
    } > {};

    template <typename Addon_T>
    struct ArgumentProvider {
        using type = extensions::scheduler::argument_provider_for_t<Addon_T>;
    };
};

template <app::builder_c Self_T, core::scheduler::decays_to_task_builder_c TaskBuilder_T>
constexpr auto plugins::Scheduler::run(this Self_T&& self, TaskBuilder_T&& task_builder)
{
    auto app{ std::forward<Self_T>(self).build() };
    using App               = decltype(app);
    using Addons            = App::Addons;
    using ArgumentProviders = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<Addons, AddonTraits<App>::template HasArgumentProvider>,
        AddonTraits<App>::template ArgumentProvider>;

    auto root_task = util::meta::apply<ArgumentProviders>(
        [&task_builder, &app]<typename... ArgumentProviders_T> {
            return core::scheduler::build(
                std::forward<TaskBuilder_T>(task_builder), ArgumentProviders_T(app)...
            );
        }
    );

    return std::invoke(root_task);
}
