#include "Renderer.hpp"

namespace engine::renderer {

Renderer::Renderer(
    const Instance&          t_instance,
    const vk::SurfaceKHR     t_surface,
    const vk::PhysicalDevice t_physical_device
)
    : m_device{ t_surface, t_physical_device },
      m_allocator{ t_instance, m_device },
      m_swapchain{ vk::UniqueSurfaceKHR{t_surface, *t_instance}, m_device}
{}

}   // namespace engine::renderer
