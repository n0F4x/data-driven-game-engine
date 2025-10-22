module;

#include <atomic>
#include <memory>
#include <type_traits>
#include <utility>

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module ddge.modules.exec.Plugin;

import ddge.modules.app.add_on_t;
import ddge.modules.app.builder_c;
import ddge.modules.app.decays_to_app_c;
import ddge.modules.exec.as_task_builder_t;
import ddge.modules.exec.converts_to_task_builder_c;
import ddge.modules.exec.Nexus;
import ddge.modules.exec.scheduler.WorkTree;
import ddge.modules.exec.provider_c;
import ddge.modules.exec.ProviderOf;
import ddge.modules.exec.wrap_as_builder;
import ddge.modules.exec.v2.as_task_blueprint;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.exec.v2.gatherers.WaitAll;
import ddge.modules.exec.v2.TaskBlueprint;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskBuilderBundle;
import ddge.modules.exec.v2.TaskBundle;
import ddge.modules.exec.v2.TaskHub;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.meta.type_traits.type_list.type_list_filter;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.TypeList;

namespace ddge::exec {

export class Plugin {
public:
    template <app::builder_c Self_T, converts_to_task_builder_c TaskBuilder_T>
    auto run(this Self_T&&, TaskBuilder_T&& task_builder)
        -> as_task_builder_t<TaskBuilder_T>::Result;

    template <app::builder_c Self_T, v2::convertible_to_TaskBlueprint_c<void> TaskBlueprint_T>
    auto run(this Self_T&&, TaskBlueprint_T&& task_blueprint) -> void;
};

}   // namespace ddge::exec

template <typename App_T>
struct AddonTraits {
    template <typename Addon_T>
        struct HasAccessorProvider : std::bool_constant < requires {
        ddge::exec::provider_c<ddge::exec::provider_for_t<Addon_T>, App_T>;
    } > {};

    template <typename Addon_T>
    struct AccessorProvider {
        using type = ddge::exec::provider_for_t<Addon_T>;
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

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilder<void>&& task_builder)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder);
}

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilderBundle<void>&& task_builder_bundle)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder_bundle).sync(ddge::exec::v2::WaitAllBuilder{});
}

template <
    ddge::app::builder_c                                 Self_T,
    ddge::exec::v2::convertible_to_TaskBlueprint_c<void> TaskBlueprint_T>
auto ddge::exec::Plugin::run(this Self_T&& self, TaskBlueprint_T&& task_blueprint) -> void
{
    auto app{ std::forward<Self_T>(self).build() };
    using App                       = decltype(app);
    using Addons                    = App::Addons;
    using AccessorProvidersTypeList = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<Addons, AddonTraits<App>::template HasAccessorProvider>,
        AddonTraits<App>::template AccessorProvider>;

    [&task_blueprint, &app]<typename... AccessorProviders_T>   //
        (util::TypeList<AccessorProviders_T...>)               //
    {
        Nexus              nexus{ AccessorProviders_T{ app }... };
        v2::TaskHubBuilder task_hub_builder;

        std::atomic_bool should_stop{};
        v2::TaskBundle   root_task =
            ::sync(v2::as_task_blueprint<void>(std::move(task_blueprint)).materialize())
                .build(nexus, task_hub_builder, [&should_stop](const v2::TaskHubProxy&) {
                    should_stop = true;
                });

        const std::unique_ptr<v2::TaskHub> task_hub{ std::move(task_hub_builder).build() };
        root_task(v2::TaskHubProxy{ *task_hub });

        while (!should_stop.load()) {
            if (!task_hub->try_execute_a_main_only_task()) {
                task_hub->try_execute_a_generic_task(0);
            }
        }
    }(AccessorProvidersTypeList{});
}
