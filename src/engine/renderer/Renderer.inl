#include <SFML/Window/Vulkan.hpp>

#include "engine/core/vulkan.hpp"

namespace engine {

namespace internal {

template <renderer::WindowConcept WindowType>
auto create_surface(const vk::raii::Instance& t_instance, WindowType& t_window)
    -> vk::raii::SurfaceKHR
{
    VkSurfaceKHR surface{};

    if (!t_window.createVulkanSurface(*t_instance, surface, nullptr)) {
        throw std::runtime_error("Failed to create window surface");
    }

    return { t_instance, surface, nullptr };
}

}   // namespace internal

template <renderer::WindowConcept WindowType>
Renderer<WindowType>::Renderer(const vk::ApplicationInfo& t_app_info,
                               Window&                    t_window)
    : m_instance{ utils::create_instance(
        t_app_info,
        utils::create_validation_layers(),
        sf::Vulkan::getGraphicsRequiredInstanceExtensions()) },
      m_pimpl{ m_instance, internal::create_surface(m_instance, t_window) },
      m_swap_chain{ t_window }
{
}

}   // namespace engine
