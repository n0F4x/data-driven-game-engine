#pragma once

#include <type_traits>

#include "store/Store.hpp"

namespace app {

class App;

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner, App&, Args...>;

class App {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]]
    static auto create() -> Builder;

    explicit App(Builder&& t_builder);

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename... Args>
    auto run(RunnerConcept<Args...> auto&& t_runner, Args&&... t_args)
        -> std::invoke_result_t<decltype(t_runner), App&, Args...>;

    [[nodiscard]]
    auto store() noexcept -> Store&;
    [[nodiscard]]
    auto store() const noexcept -> const Store&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

}   // namespace app

#include "app/App.inl"
