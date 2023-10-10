#pragma once

#include <type_traits>

#include "engine/utility/Context.hpp"

namespace engine {

class App;

template <typename Runner, typename... Args>
concept RunnerConcept = std::invocable<Runner, App&, Args...>;

class App {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Context = utils::Context;

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

    [[nodiscard]] auto context() noexcept -> Context&;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Builder;

    ///*************///
    ///  Variables  ///
    ///*************///
    Context m_context;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit App(Context&& t_context) noexcept;
};

}   // namespace engine

#include "App.inl"
