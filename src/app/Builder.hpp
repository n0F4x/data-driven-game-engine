template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, App&>;

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner, App, Args...>;

class App::Builder {
public:
    template <PluginConcept Plugin>
    auto add_plugin() -> Builder&;
    template <PluginConcept Plugin>
    auto add_plugin(Plugin&& plugin) -> Builder&;

    [[nodiscard]]
    auto build() -> App;

    template <typename... Args>
    auto run(RunnerConcept<Args...> auto&& runner, Args&&... args)
        -> std::invoke_result_t<decltype(runner), App, Args...>;


private:
    std::vector<std::function<void(App& app)>> m_plugins;
    std::vector<std::type_index>               m_plugin_ids;
};

#include "Builder.inl"
