template <typename Plugin, typename... Args>
concept PluginConcept = std::invocable<Plugin, App::Builder&, Args...>;

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner, App, Args...>;

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Plugin>
    auto add_plugin(auto&&... t_args) && -> Builder;

    template <typename... Args>
    auto add_plugin(PluginConcept<Args...> auto&& t_plugin, Args&&... t_args) && -> Builder;

    [[nodiscard]]
    auto store() noexcept -> Store&;
    [[nodiscard]]
    auto store() const noexcept -> const Store&;

    [[nodiscard]]
    auto build() && noexcept -> App;

    template <typename... Args>
    auto
        run(RunnerConcept<Args...> auto&& t_runner, Args&&... t_args
        ) && -> std::invoke_result_t<decltype(t_runner), App, Args...>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

#include "Builder.inl"
