#pragma once

#include <functional>
#include <optional>
#include <type_traits>

#include "engine/utility/Result.hpp"

#include "config.hpp"

namespace engine {

template <typename Func>
concept Runner = std::
    is_nothrow_invocable_v<Func, app::config::Renderer&, app::config::Window&>;

class App {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Renderer = app::config::Renderer;
    using Window   = app::config::Window;

    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto run(Runner auto t_runner) noexcept -> void;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Builder;

    ///*************///
    ///  Variables  ///
    ///*************///
    Renderer                                   m_renderer;
    Window                                     m_window;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit App(Renderer&& t_renderer, Window&& t_window) noexcept;
};

class App::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() && noexcept -> std::optional<App>;
    auto build_and_run(Runner auto t_runner) && noexcept -> Result;

    auto set_window(Window&& t_window) && noexcept -> Builder;
    auto set_window(const Window::Builder& t_window_builder) && noexcept
        -> Builder;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend App;

    ///*************///
    ///  Variables  ///
    ///*************///
    std::optional<Window> m_window;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    Builder() noexcept = default;
};

}   // namespace engine

#include "App.inl"
