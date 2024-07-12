#include <spdlog/spdlog.h>

template <typename... Args>
auto App::Builder::run(RunnerConcept<Args...> auto&& runner, Args&&... args)
    -> std::invoke_result_t<decltype(runner), App, Args...>
{
    SPDLOG_INFO("App is running");
    return std::invoke(
        std::forward<decltype(runner)>(runner), build(), std::forward<Args>(args)...
    );
}

template <PluginConcept Plugin>
auto App::Builder::add_plugin() -> Builder&
{
    return add_plugin(Plugin{});
}

template <PluginConcept Plugin>
auto App::Builder::add_plugin(Plugin&& plugin) -> Builder&
{
    if constexpr (requires(Plugin t) {
                      {
                          t.dependencies()
                      } -> std::ranges::range;
                  })
    {
        for (auto&& dependency : plugin.dependencies()) {
            if (std::ranges::none_of(m_plugin_ids, [&](std::type_index existing_plugin_id) {
                    return dependency == existing_plugin_id;
                }))
            {
                throw std::runtime_error{ std::format(
                    "{} depends on not (yet) added plugin \"{}\"",
                    typeid(plugin).name(),
                    dependency.name()
                ) };
            }
        }
    }

    m_plugins.emplace_back(std::forward<Plugin>(plugin));
    m_plugin_ids.push_back(typeid(plugin));

    return *this;
}
