#pragma once

#include <type_traits>

#include "engine/utility/Store.hpp"

namespace engine {

class App;

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner, App&, Args...>;

class App {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Store = utils::Store;

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
    auto run(RunnerConcept<Args...> auto&& t_runner, Args&&... t_args) noexcept
        -> void;

    [[nodiscard]] auto store() noexcept -> Store&;

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
