module;

#include <atomic>
#include <memory>
#include <type_traits>
#include <utility>

export module ddge.modules.execution.Plugin;

import ddge.modules.app.add_on_t;
import ddge.modules.app.builder_c;
import ddge.modules.app.decays_to_app_c;
import ddge.modules.execution.as_task_builder_t;
import ddge.modules.execution.converts_to_task_builder_c;
import ddge.modules.execution.Nexus;
import ddge.modules.execution.scheduler.WorkTree;
import ddge.modules.execution.provider_c;
import ddge.modules.execution.ProviderOf;
import ddge.modules.execution.wrap_as_builder;
import ddge.modules.execution.v2.Task;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskHub;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

import ddge.utility.meta.type_traits.type_list.type_list_filter;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.TypeList;

namespace ddge::exec {

export class Plugin {
public:
    template <app::builder_c Self_T, converts_to_task_builder_c TaskBuilder_T>
    auto run(this Self_T&&, TaskBuilder_T&& task_builder)
        -> as_task_builder_t<TaskBuilder_T>::Result;

    template <app::builder_c Self_T>
    auto run(this Self_T&&, v2::TaskBuilder<void>&& task_builder) -> void;
};

}   // namespace ddge::exec

template <typename App_T>
struct AddonTraits {
    template <typename Addon_T>
        struct HasAccessorProvider : std::bool_constant < requires {
        ddge::exec::provider_c<ddge::exec::provider_of_t<Addon_T>, App_T>;
    } > {};

    template <typename Addon_T>
    struct AccessorProvider {
        using type = ddge::exec::provider_of_t<Addon_T>;
    };
};

template <ddge::app::builder_c Self_T, ddge::exec::converts_to_task_builder_c TaskBuilder_T>
auto ddge::exec::Plugin::run(this Self_T&& self, TaskBuilder_T&& task_builder)
    -> as_task_builder_t<TaskBuilder_T>::Result
{
    auto app{ std::forward<Self_T>(self).build() };
    using App               = decltype(app);
    using Addons            = App::Addons;
    using AccessorProviders = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<Addons, AddonTraits<App>::template HasAccessorProvider>,
        AddonTraits<App>::template AccessorProvider>;

    return
        [&task_builder,
         &app]<typename... AccessorProviders_T>(util::TypeList<AccessorProviders_T...>) {
            Nexus nexus{ AccessorProviders_T{ app }... };

            return wrap_as_builder(task_builder).build(nexus)();
        }(AccessorProviders{});
}

template <ddge::app::builder_c Self_T>
auto ddge::exec::Plugin::run(this Self_T&& self, v2::TaskBuilder<void>&& task_builder)
    -> void
{
    auto app{ std::forward<Self_T>(self).build() };
    using App                       = decltype(app);
    using Addons                    = App::Addons;
    using AccessorProvidersTypeList = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<Addons, AddonTraits<App>::template HasAccessorProvider>,
        AddonTraits<App>::template AccessorProvider>;

    [&task_builder, &app]<typename... AccessorProviders_T>   //
        (util::TypeList<AccessorProviders_T...>)             //
    {
        Nexus              nexus{ AccessorProviders_T{ app }... };
        v2::TaskHubBuilder task_hub_builder;

        std::atomic_bool should_stop{};
        const v2::Task   task =
            std::move(task_builder)
                .build(nexus, task_hub_builder, [&should_stop](const v2::TaskHubProxy&) {
                    should_stop = true;
                });

        const std::unique_ptr<v2::TaskHub> task_hub{ std::move(task_hub_builder).build() };

        task.schedule(v2::TaskHubProxy{ *task_hub });

        while (!should_stop.load()) {
            if (!task_hub->try_execute_one_main_only_work()) {
                task_hub->try_execute_one(0);
            }
        }
    }(AccessorProvidersTypeList{});
}
