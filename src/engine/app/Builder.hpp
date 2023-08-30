#pragma once

#include "engine/utility/Result.hpp"

#include "App.hpp"
#include "Plugin.hpp"

namespace engine {

template <typename Plugin>
concept PluginConcept = requires(Plugin t_plugin, App::Context& t_context) {
    {
        t_plugin.set_context(t_context)
    };
};

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() && noexcept -> std::optional<App>;
    auto build_and_run(Runner<App> auto t_runner) && noexcept -> Result;

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
