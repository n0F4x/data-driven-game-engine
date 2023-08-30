#include "Window.hpp"

namespace engine::window {

/////////////////////////////////
///---------------------------///
///  Window   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////

auto Window::create(
    const sf::VideoMode& t_video_mode,
    const sf::String&    t_title,
    Style                t_style
) noexcept -> std::optional<Window>
{
    try {
        return Window{ t_video_mode,
                       t_title,
                       static_cast<sf::Uint32>(t_style) };
    } catch (...) {
        return std::nullopt;
    }
}

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

Window::Window(
    const sf::VideoMode& t_video_mode,
    const sf::String&    t_title,
    sf::Uint32           t_style
) noexcept(false)
    : m_impl{ std::make_unique<sf::WindowBase>(t_video_mode, t_title, t_style) }
{}

}   // namespace engine::window
