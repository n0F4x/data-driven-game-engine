#pragma once

#include <tl/optional.hpp>

#include "engine/common/Plugin.hpp"

#include "App.hpp"

namespace engine {

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() && noexcept -> tl::optional<App>;

    template <typename... Args>
    auto build_and_run(
        RunnerConcept<Args...> auto&& t_runner,
        Args&&... t_args
    ) && noexcept
        -> tl::optional<decltype(std::declval<App>().run(
            std::forward<decltype(t_runner)>(t_runner),
            std::forward<decltype(t_args)>(t_args)...
        ))>;


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
    Store m_store{};

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    Builder() noexcept = default;
};

}   // namespace engine

#include "Builder.inl"
