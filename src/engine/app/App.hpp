#pragma once

#include <type_traits>

#include "Store.hpp"

namespace engine {

class App;

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner, App&, Args...>;

class App {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;
    class Instance;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename... Args>
    auto run(RunnerConcept<Args...> auto&& t_runner, Args&&... t_args)
        -> std::invoke_result_t<decltype(t_runner), App&, Args...>;

    [[nodiscard]] auto store() noexcept -> Store&;
    [[nodiscard]] auto store() const noexcept -> const Store&;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Builder;

    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit App(Store&& t_store) noexcept;
};

}   // namespace engine

#include "App.inl"
