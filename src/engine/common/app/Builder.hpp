#pragma once

#include "engine/common/utility/Result.hpp"

#include "App.hpp"
#include "Plugin.hpp"

namespace engine {

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() && noexcept -> std::optional<App>;
    auto build_and_run(const Runner<App> auto& t_runner) && noexcept -> Result;

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
