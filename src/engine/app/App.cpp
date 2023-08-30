#include "App.hpp"

#include "engine/renderer/defaults.hpp"

namespace engine {

//////////////////////////////
///----------------------- ///
///  App   IMPLEMENTATION  ///
///------------------------///
//////////////////////////////
App::App(Renderer&& t_renderer, Window&& t_window) noexcept
    : m_renderer{ std::move(t_renderer) },
      m_window{ std::move(t_window) }
{}

auto App::create() noexcept -> App::Builder
{
    return Builder{};
}

///////////////////////////////////////
///---------------------------------///
///  App::Builder   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////
auto App::Builder::build() && noexcept -> std::optional<App>
{
    if (!m_window.has_value()) {
        m_window = Window::create().build();
    }

    if (!m_window.has_value()) {
        return std::nullopt;
    }

    auto renderer{ renderer::defaults::create<Renderer>(*m_window) };
    if (!renderer.has_value()) {
        return std::nullopt;
    }
    renderer->set_framebuffer_size(vk::Extent2D{
        .width  = m_window->framebuffer_size().x,
        .height = m_window->framebuffer_size().y });

    return App{ std::move(*renderer), std::move(*m_window) };
}

auto App::Builder::set_window(Window&& t_window) && noexcept -> Builder
{
    m_window = std::move(t_window);
    return std::move(*this);
}

auto App::Builder::set_window(const Window::Builder& t_window_builder
) && noexcept -> Builder
{
    m_window = t_window_builder.build();
    return std::move(*this);
}

}   // namespace engine
