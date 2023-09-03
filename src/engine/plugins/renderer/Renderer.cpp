#include "Renderer.hpp"

namespace engine::renderer {

auto Renderer::rank_physical_device(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR
) noexcept -> size_t
{
    auto device_type{ t_physical_device.getProperties().deviceType };

    if (device_type == vk::PhysicalDeviceType::eDiscreteGpu) {
        return 100;
    }
    if (device_type == vk::PhysicalDeviceType::eIntegratedGpu) {
        return 10;
    }
    return 0;
}

Renderer::Renderer(
    vk::Instance                      t_instance,
    vk::DebugUtilsMessengerEXT        t_debug_messenger,
    vk::SurfaceKHR                    t_surface,
    vk::Extent2D                      t_framebuffer_size,
    std::vector<vk::PhysicalDevice>&& t_adequate_devices,
    Device&&                          t_device,
    RenderFrame&&                     t_render_frame
)
    : m_instance{ t_instance, t_debug_messenger },
      m_framebuffer_size{ std::make_unique<std::atomic<vk::Extent2D>>(t_framebuffer_size) },
      m_adequate_devices{ std::move(t_adequate_devices) },
      m_device{ std::move(t_device) },
      m_swapchain{
          vulkan::Surface{*m_instance, t_surface},
          m_device,
          [this](){ return m_framebuffer_size->load(); }
      },
      m_render_frame{ std::move(t_render_frame) }
{}

auto Renderer::set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    m_framebuffer_size->store(t_framebuffer_size);
}

}   // namespace engine::renderer
