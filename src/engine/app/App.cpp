#include "App.hpp"

namespace engine {

//////////////////////////////
///----------------------- ///
///  App   IMPLEMENTATION  ///
///------------------------///
//////////////////////////////
App::App(Builder&& t_builder)
    : m_renderer{ t_builder.renderer().build(m_window) },
      m_runner{ t_builder.runner() },
      m_window{ t_builder.window() } {}

void App::run() {
    if (m_runner) {
        m_runner(m_renderer, m_window);
    }
}

auto App::create() noexcept -> App::Builder {
    return Builder{};
}

///////////////////////////////////////
///---------------------------------///
///  App::Builder   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////
auto App::Builder::build() -> App {
    return App{ std::move(*this) };
}

auto App::Builder::set_renderer(
    const Renderer::Builder& t_renderer_builder) noexcept -> App::Builder& {
    m_renderer_builder = t_renderer_builder;
    return *this;
}

auto App::Builder::set_runner(Runner&& t_runner) noexcept -> Builder& {
    m_runner = std::move(t_runner);
    return *this;
}

auto App::Builder::set_window(const Window::Builder& t_window_builder) noexcept
    -> App::Builder& {
    m_window_builder = t_window_builder;
    return *this;
}

auto App::Builder::runner() noexcept -> App::Runner {
    return std::move(m_runner);
}

auto App::Builder::window() noexcept -> const Window::Builder& {
    return m_window_builder;
}

auto App::Builder::renderer() noexcept -> const Renderer::Builder& {
    return m_renderer_builder;
}

}   // namespace engine
