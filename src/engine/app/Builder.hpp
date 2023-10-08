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
    auto build_and_run(RunnerConcept<App> auto&& t_runner) && noexcept
        -> Result;

    template <PluginConcept Plugin>
    auto add_plugin() && noexcept -> Builder;
    template <typename Plugin>
    auto add_plugin(auto&&... t_args) && noexcept -> Builder;
    auto add_plugin(PluginConcept auto&& t_plugin) && noexcept -> Builder;

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
