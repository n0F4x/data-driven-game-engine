module;

#include <type_traits>
#include <utility>

export module modules.scheduler.Plugin;

import modules.app.builder_c;
import modules.scheduler.converts_to_task_builder_c;
import modules.scheduler.Nexus;
import modules.scheduler.provider_c;
import modules.scheduler.ProviderOf;
import modules.scheduler.wrap_as_builder;

import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.TypeList;

namespace modules::scheduler {

export class Plugin {
public:
    template <app::builder_c Self_T, converts_to_task_builder_c TaskBuilder_T>
    constexpr auto run(this Self_T&&, TaskBuilder_T&& task_builder);
};

}   // namespace modules::scheduler

template <typename App_T>
struct AddonTraits {
    template <typename Addon_T>
        struct HasAccessorProvider : std::bool_constant < requires {
        modules::scheduler::provider_c<modules::scheduler::provider_of_t<Addon_T>, App_T>;
    } > {};

    template <typename Addon_T>
    struct AccessorProvider {
        using type = modules::scheduler::provider_of_t<Addon_T>;
    };
};

template <
    modules::app::builder_c                        Self_T,
    modules::scheduler::converts_to_task_builder_c TaskBuilder_T>
constexpr auto
    modules::scheduler::Plugin::run(this Self_T&& self, TaskBuilder_T&& task_builder)
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
            Nexus nexus{ AccessorProviders_T(app)... };

            return wrap_as_builder(task_builder).build(nexus)();
        }(AccessorProviders{});
}
