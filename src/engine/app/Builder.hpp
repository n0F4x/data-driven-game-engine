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
    auto build_and_run(const RunnerConcept<App> auto& t_runner) && noexcept
        -> Result;

    template <PluginConcept Plugin>
    auto add_plugin() && noexcept -> Builder;
    auto add_plugin(const PluginConcept auto& t_plugin) && noexcept -> Builder;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend App;

    ///*************///
    ///  Variables  ///
    ///*************///
    Context m_context{ std::allocator<entt::id_type>{} };

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    Builder() noexcept = default;
};

}   // namespace engine

#include "Builder.inl"
