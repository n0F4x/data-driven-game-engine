#pragma once

#include <spdlog/spdlog.h>

#include "core/store/StoreView.hpp"
#include "core/utility/meta/functional.hpp"
#include "core/utility/meta/tuple-like.hpp"
#include "core/utility/tuple.hpp"

template <typename Self, PluginConcept Plugin>
auto App::Builder::use(this Self&& self, Plugin&& plugin) -> Self
{
    if constexpr (requires(StoreView plugins) { plugin.setup(plugins); }) {
        plugin.setup(StoreView{ self.m_plugins });
    }

    self.m_invocations.emplace_back(self.m_plugins.template emplace<std::decay_t<Plugin>>(
        std::forward<Plugin>(plugin)
    ));

    return std::forward<Self>(self);
}

template <typename Self, ModifierConcept Modifier>
auto App::Builder::apply(this Self&& self, Modifier&& modifier) -> Self
{
    std::invoke(std::forward<Modifier>(modifier), self);

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
    using RequiredResourcesTuple =
        core::meta::arguments_of_t<std::remove_pointer_t<std::decay_t<Plugin>>>;
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
          using ResourceType =
              core::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Plugin>>>;
          app.resources.emplace<ResourceType>(std::apply(
              std::any_cast<Plugin&>(erased_plugin_ref),
              details::gather_resources<Plugin>(app)
          ));
      } }
{}
