#pragma once

#include "App.hpp"
#include "Plugin.hpp"

namespace app {

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() && noexcept -> App;

    template <typename... Args>
    auto build_and_run(
        RunnerConcept<Args...> auto&& t_runner,
        Args&&... t_args
    ) && -> decltype(std::declval<App>()
                         .run(
                             std::forward<decltype(t_runner)>(t_runner),
                             std::forward<decltype(t_args)>(t_args)...
                         ));


    template <typename Plugin>
    auto add_plugin(auto&&... t_args) && -> Builder;

    template <typename... Args>
    auto add_plugin(PluginConcept<Args...> auto&& t_plugin, Args&&... t_args) && -> Builder;

    [[nodiscard]] auto store() noexcept -> Store&;
    [[nodiscard]] auto store() const noexcept -> const Store&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store{};
};

}   // namespace engine

#include "Builder.inl"
