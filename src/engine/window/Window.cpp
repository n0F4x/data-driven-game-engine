#include "Window.hpp"

namespace engine::window {

/////////////////////////////////
///---------------------------///
///  Window   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////

Window::Window(
    const sf::VideoMode& t_video_mode,
    const sf::String&    t_title,
    sf::Uint32           t_style
) noexcept(false)
    : m_impl{ std::make_unique<sf::WindowBase>(t_video_mode, t_title, t_style) }
{}

auto Window::framebuffer_size() const noexcept -> sf::Vector2u
{
    return m_impl->getSize();
}

auto Window::create_vulkan_surface(
    VkInstance                   t_instance,
    const VkAllocationCallbacks* t_allocator
) noexcept -> std::optional<VkSurfaceKHR>
{
    if (!sf::Vulkan::isAvailable()) {
        return std::nullopt;
    }

    VkSurfaceKHR surface{};

    try {
        if (m_impl->createVulkanSurface(t_instance, surface, t_allocator)) {
            return surface;
        }
    } catch (...) {
        return std::nullopt;
    }

    return std::nullopt;
}

auto Window::create() noexcept -> Builder
{
    return Builder{};
}

//////////////////////////////////////////
///------------------------------------///
///  Window::Builder   IMPLEMENTATION  ///
///------------------------------------///
//////////////////////////////////////////

auto Window::Builder::build() const noexcept -> std::optional<Window>
{
    try {
        return Window{ m_video_mode, m_title, m_style };
    } catch (...) {
        return std::nullopt;
    }
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
