#pragma once

#include <functional>

#include "config.hpp"

namespace engine {

class App {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Renderer = app::config::Renderer;
    using Runner =
        std::function<void(app::config::Renderer&, app::config::Window&)>;
    using Window = app::config::Window;

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
    [[nodiscard]] explicit App(Builder&& t_builder);

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run();

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Window   m_window;
    Renderer m_renderer;
    Runner   m_runner;
};

class App::Builder final {
    ///-----------///
    ///  Friends  ///
    ///-----------///
    friend App;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Builder() = default;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() -> App;

    auto set_runner(Runner&& t_runner) noexcept -> Builder&;
    auto set_window(const Window::Builder& t_window_builder) noexcept
        -> Builder&;

    [[nodiscard]] auto release_runner() noexcept -> Runner;
    [[nodiscard]] auto window() noexcept -> const Window::Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Runner          m_runner;
    Window::Builder m_window_builder{ Window::create() };
};

}   // namespace engine
