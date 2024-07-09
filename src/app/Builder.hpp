template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, Store&, Args...>;

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner, App, Args...>;

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Context, typename Self, typename... Args>
    auto store(this Self&&, Args&&... args) -> Self;

    template <typename Plugin, typename Self>
    auto add_plugin(this Self&&, auto&&... t_args) -> Self;
    template <typename Self, typename... Args>
    auto add_plugin(this Self&&, PluginConcept<Args...> auto&& t_plugin, Args&&... t_args)
        -> Self;

    [[nodiscard]]
    auto build() && noexcept -> App;

    template <typename... Args>
    auto
        run(RunnerConcept<Args...> auto&& runner, Args&&... args
        ) && -> std::invoke_result_t<decltype(runner), App, Args...>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

#include "Builder.inl"
