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

    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend Builder;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit App(Renderer&& t_renderer, Window&& t_window) noexcept;

public:
    App(App&&) noexcept = default;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(App&&) noexcept -> App& = default;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto run(Runner auto t_runner) noexcept -> void;

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Renderer m_renderer;
    Window   m_window;
};

class App::Builder {
    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend App;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Builder() noexcept = default;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() && noexcept -> std::optional<App>;
    auto build_and_run(Runner auto t_runner) && noexcept -> Result;

    auto set_window(std::optional<Window> t_window) && noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::optional<Window> m_window;
};

}   // namespace engine

#include "App.inl"
