#include "Window.hpp"

namespace engine {

/////////////////////////////////
///---------------------------///
///  Window   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////
Window::Window(const Builder& t_builder)
    : m_pimpl{ t_builder.video_mode(), t_builder.title(), t_builder.style() }
{}

auto Window::create() noexcept -> Builder
{
    return Builder{};
}

sf::Vector2u Window::getSize() const
{
    return m_pimpl.getSize();
}

bool Window::createVulkanSurface(
    const VkInstance&            t_instance,
    VkSurfaceKHR&                t_surface,
    const VkAllocationCallbacks* t_allocator
)
{
    return m_pimpl.createVulkanSurface(t_instance, t_surface, t_allocator);
}

//////////////////////////////////////////
///------------------------------------///
///  Window::Builder   IMPLEMENTATION  ///
///------------------------------------///
//////////////////////////////////////////
auto Window::Builder::build() const -> Window
{
    return Window{ *this };
}

auto Window::Builder::set_video_mode(const sf::VideoMode& t_video_mode) noexcept
    -> Window::Builder&
{
    m_video_mode = t_video_mode;
    return *this;
}

auto Window::Builder::set_title(const sf::String& t_title) noexcept
    -> Window::Builder&
{
    m_title = t_title;
    return *this;
}

auto Window::Builder::set_style(sf::Uint32 t_style) noexcept -> Window::Builder&
{
    m_style = t_style;
    return *this;
}

auto Window::Builder::style() const noexcept -> sf::Uint32
{
    return m_style;
}

auto Window::Builder::title() const noexcept -> const sf::String&
{
    return m_title;
}

auto Window::Builder::video_mode() const noexcept -> const sf::VideoMode&
{
    return m_video_mode;
}

}   // namespace engine
