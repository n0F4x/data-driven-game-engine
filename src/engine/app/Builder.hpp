#pragma once

#include "engine/utility/Result.hpp"

#include "App.hpp"
#include "Plugin.hpp"

namespace engine {

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() && noexcept -> std::optional<App>;

    template <typename... Args>
    auto build_and_run(
        RunnerConcept<Args...> auto&& t_runner,
        Args&&... t_args
    ) && noexcept -> Result;

    
    template <typename Plugin>
    auto add_plugin(auto&&... t_args) && noexcept -> Builder;

    template <typename... Args>
    auto add_plugin(
        PluginConcept<Args...> auto&& t_plugin,
        Args&&... t_args
    ) && noexcept -> Builder;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend App;

    ///*************///
    ///  Variables  ///
    ///*************///
    Context m_context{};

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    Builder() noexcept = default;
};

}   // namespace engine

#include "Builder.inl"
