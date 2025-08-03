module;

#include <functional>
#include <type_traits>
#include <utility>

export module plugins.scheduler;

import app.builder_c;

import core.scheduler;

import extensions.scheduler.provider_c;
import extensions.scheduler.ProviderOf;

import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.TypeList;

namespace plugins {

export class Scheduler {
public:
    template <
        app::builder_c                                  Self_T,
        core::scheduler::converts_to_task_builder_c TaskBuilder_T>
    constexpr auto run(this Self_T&&, TaskBuilder_T&& task_builder);
};

export inline constexpr Scheduler scheduler;

}   // namespace plugins

template <typename App_T>
struct AddonTraits {
    template <typename Addon_T>
        struct HasAccessorProvider : std::bool_constant < requires {
        extensions::scheduler::provider_c<
            extensions::scheduler::provider_of_t<Addon_T>,
            App_T>;
    } > {};

    template <typename Addon_T>
    struct AccessorProvider {
        using type = extensions::scheduler::provider_of_t<Addon_T>;
    };
};

template <app::builder_c Self_T, core::scheduler::converts_to_task_builder_c TaskBuilder_T>
constexpr auto plugins::Scheduler::run(this Self_T&& self, TaskBuilder_T&& task_builder)
{
    auto app{ std::forward<Self_T>(self).build() };
    using App               = decltype(app);
    using Addons            = App::Addons;
    using AccessorProviders = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<Addons, AddonTraits<App>::template HasAccessorProvider>,
        AddonTraits<App>::template AccessorProvider>;

    return [&task_builder,
            &app]<typename... AccessorProviders_T>(util::TypeList<AccessorProviders_T...>) {
        core::scheduler::Nexus nexus{ AccessorProviders_T(app)... };

        return core::scheduler::wrap_as_builder(task_builder).build(nexus)();
    }(AccessorProviders{});
}
