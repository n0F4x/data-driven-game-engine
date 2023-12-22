#include "Window.hpp"

#include <spdlog/spdlog.h>

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
)
    : m_impl{ std::make_unique<sf::WindowBase>(t_video_mode, t_title, t_style) }
{}

auto Window::operator->() const noexcept -> sf::WindowBase*
{
    return m_impl.operator->();
}

auto Window::framebuffer_size() const noexcept -> sf::Vector2u
{
    return m_impl->getSize();
}

auto Window::create_vulkan_surface(
    VkInstance                   t_instance,
    const VkAllocationCallbacks* t_allocator
) -> VkSurfaceKHR
{
    VkSurfaceKHR surface{};

    if (!m_impl->createVulkanSurface(t_instance, surface, t_allocator)) {
        SPDLOG_ERROR("sf::WindowBase::createVulkanSurface failed");
    }

    return surface;
}

}   // namespace engine::window
