#include <spdlog/spdlog.h>

#include <core/meta/functional.hpp>
#include <core/meta/tuple-like.hpp>
#include <core/utility/tuple.hpp>

template <PluginConcept Plugin, typename Self, typename... Args>
auto App::Builder::append(this Self&& self, Args&&... args) -> Self
{
    return std::forward<Self>(self).append(
        std::remove_cvref_t<Plugin>(std::forward<Args>(args)...)
    );
}

template <PluginConcept Plugin, typename Self>
auto App::Builder::append(this Self&& self, Plugin&& plugin) -> Self
{
    if constexpr (requires(Plugin t, StoreView plugins) { t.setup(plugins); }) {
        plugin.setup(StoreView{ self.m_plugins });
    }

    Plugin plugin_ref{ self.m_plugins.template emplace<Plugin>(std::forward<Plugin>(plugin
    )) };
    self.m_invocations.emplace_back(plugin_ref);

    return std::forward<Self>(self);
}

template <PluginGroupConcept PluginGroup, typename Self, typename... Args>
auto App::Builder::append_group(this Self&& self, Args&&... args) -> Self
{
    return std::forward<Self>(self).append_group(PluginGroup{
        std::forward<Args>(args)... });
}

template <PluginGroupConcept PluginGroup, typename Self>
auto App::Builder::append_group(this Self&& self, PluginGroup&& plugin_group) -> Self
{
    std::invoke(std::forward<PluginGroup>(plugin_group), self);

    return std::forward<Self>(self);
}

template <typename Runner, typename... Args>
    requires(RunnerConcept<Runner, Args...>)
auto App::Builder::run(Runner&& runner, Args&&... args)
    -> std::invoke_result_t<Runner&&, App&&, Args&&...>
{
    SPDLOG_INFO("App is running");

    return std::invoke(std::forward<Runner>(runner), build(), std::forward<Args>(args)...);
}

namespace details {

template <typename Plugin>
auto gather_resources(App& app)
{
    using RequiredResourcesTuple = decltype(core::utils::remove_first(
        std::declval<typename core::meta::arguments_t<Plugin>>()
    ));
    return core::utils::generate_tuple<RequiredResourcesTuple>(
        [&app]<typename Resource>() -> Resource {
            return app.resources.at<std::remove_cvref_t<Resource>>();
        }
    );
}

}   // namespace details

template <typename Plugin>
App::Builder::PluginInvocation::PluginInvocation(Plugin& plugin_ref)
    : m_plugin_ref{ plugin_ref },
      m_invocation{ [](std::any& erased_plugin_ref, App& app) {
          std::apply(
              std::any_cast<Plugin&>(erased_plugin_ref),
              std::tuple_cat(std::tuple<App&>(app), details::gather_resources<Plugin>(app))
          );
      } }
{}
