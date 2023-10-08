#pragma once

#include <functional>
#include <optional>
#include <type_traits>

#include "engine/utility/Context.hpp"

namespace engine {

template <typename Func, class App>
concept RunnerConcept = std::is_nothrow_invocable_v<Func, App&>;

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
    auto run(RunnerConcept<App> auto&& t_runner) noexcept -> void;

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
