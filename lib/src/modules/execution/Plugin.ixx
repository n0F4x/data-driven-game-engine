module;

#include <type_traits>
#include <utility>

export module ddge.modules.execution.Plugin;

import ddge.modules.app.add_on_t;
import ddge.modules.app.builder_c;
import ddge.modules.app.decays_to_app_c;
import ddge.modules.execution.Addon;
import ddge.modules.execution.as_task_builder_t;
import ddge.modules.execution.converts_to_task_builder_c;
import ddge.modules.execution.Nexus;
import ddge.modules.execution.scheduler.WorkHub;
import ddge.modules.execution.provider_c;
import ddge.modules.execution.ProviderOf;
import ddge.modules.execution.wrap_as_builder;

import ddge.utility.meta.type_traits.type_list.type_list_filter;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.TypeList;

namespace ddge::exec {

export class Plugin {
public:
    constexpr explicit Plugin(
        WorkHub::SizeCategory work_hub_size = WorkHub::SizeCategory::eDefault
    );

    template <app::builder_c Self_T, converts_to_task_builder_c TaskBuilder_T>
    auto run(this Self_T&&, TaskBuilder_T&& task_builder)
        -> as_task_builder_t<TaskBuilder_T>::Result;

    template <ddge::app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) -> app::add_on_t<App_T, Addon>;

private:
    WorkHub::SizeCategory m_work_hub_size;
};

}   // namespace ddge::exec

constexpr ddge::exec::Plugin::Plugin(const WorkHub::SizeCategory work_hub_size)
    : m_work_hub_size{ work_hub_size }
{}

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

template <ddge::app::decays_to_app_c App_T>
auto ddge::exec::Plugin::build(App_T&& app) -> app::add_on_t<App_T, Addon>
{
    return std::forward<App_T>(app).add_on(Addon{ .work_hub{ m_work_hub_size } });
}
