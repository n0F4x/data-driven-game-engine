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
    using Renderer = app::config::Renderer;
    using Runner =
        std::function<void(app::config::Renderer&, app::config::Window&)>;
    using Window = app::config::Window;

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
    Renderer m_renderer;
    Runner m_runner;
    Window m_window;
};

class App::Builder final {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto build() -> App;

    auto set_renderer(const Renderer::Builder& t_renderer_builder) noexcept
        -> Builder&;
    auto set_runner(Runner&& t_runner) noexcept -> Builder&;
    auto set_window(const Window::Builder& t_window_builder) noexcept
        -> Builder&;

    [[nodiscard]] auto renderer() noexcept -> const Renderer::Builder&;
    [[nodiscard]] auto runner() noexcept -> Runner;
    [[nodiscard]] auto window() noexcept -> const Window::Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Renderer::Builder m_renderer_builder;
    Runner m_runner;
    Window::Builder m_window_builder;
};

}   // namespace engine
