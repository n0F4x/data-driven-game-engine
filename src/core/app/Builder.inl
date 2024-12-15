#pragma once

namespace core::app {

template <typename Self, PluginConcept Plugin>
auto Builder::use(this Self&& self, Plugin&& plugin) -> Self
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
auto Builder::apply(this Self&& self, Modifier&& modifier) -> Self
{
    std::invoke(std::forward<Modifier>(modifier), self);

    return std::forward<Self>(self);
}

template <typename Runner, typename... Args>
    requires(RunnerConcept<Runner, Args...>)
auto Builder::run(Runner&& runner, Args&&... args)
    -> std::invoke_result_t<Runner&&, App&&, Args&&...>
{
    SPDLOG_INFO("App is running");

    return std::invoke(std::forward<Runner>(runner), build(), std::forward<Args>(args)...);
}

template <typename Plugin>
auto gather_resources(App& app)
{
    using RequiredResourcesTuple =
        utils::meta::arguments_of_t<std::remove_pointer_t<std::decay_t<Plugin>>>;
    return utils::generate_tuple<RequiredResourcesTuple>(
        [&app]<typename Resource>() -> Resource {
            return app.resources.at<std::remove_cvref_t<Resource>>();
        }
    );
}

template <typename Plugin>
PluginInvocation::PluginInvocation(Plugin& plugin_ref)
    : m_plugin_ref{ std::ref(plugin_ref) },
      m_invocation{ [](std::any& erased_plugin_ref, App& app) {
          using ResourceType =
              utils::meta::invoke_result_of_t<std::remove_pointer_t<std::decay_t<Plugin>>>;
          app.resources.emplace<ResourceType>(std::apply(
              std::any_cast<std::reference_wrapper<Plugin>>(erased_plugin_ref),
              gather_resources<Plugin>(app)
          ));
      } }
{}

}   // namespace core::app
