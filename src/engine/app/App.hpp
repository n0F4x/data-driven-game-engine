#pragma once

#include <functional>

#include "config.hpp"

namespace engine {

class App final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Window = app::config::Window;
    using Runner = std::function<void(Window&)>;

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
    Runner m_runner;
    Window m_window;
};

class App::Builder final {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() -> App;

    auto set_runner(Runner&& t_runner) noexcept -> Builder&;
    auto set_window(const Window::Builder& t_window_builder) noexcept
        -> Builder&;

    [[nodiscard]] auto runner() noexcept -> Runner;
    [[nodiscard]] auto window() noexcept -> const Window::Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Runner m_runner;
    Window::Builder m_window_builder;
};

}   // namespace engine
