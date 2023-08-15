#include "App.hpp"

#include <thread>

namespace {

auto create_surface_callback(engine::App::Window& t_window) noexcept
{
    return [&t_window](
               vk::Instance                                t_instance,
               vk::Optional<const vk::AllocationCallbacks> t_allocator
           ) noexcept -> std::optional<vk::SurfaceKHR> {
        return t_window.create_vulkan_surface(
            t_instance, &t_allocator->operator const VkAllocationCallbacks&()
        );
    };
}

}   // namespace

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

    auto renderer{ Renderer::create(
        {},
        create_surface_callback(*m_window),
        vk::Extent2D{ .width  = m_window->framebuffer_size().x,
                      .height = m_window->framebuffer_size().y },
        std::jthread::hardware_concurrency()
    ) };
    if (!renderer.has_value()) {
        return std::nullopt;
    }

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
