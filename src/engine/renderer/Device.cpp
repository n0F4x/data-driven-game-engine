#include "Device.hpp"

namespace engine::renderer {

/////////////////////////////////
///---------------------------///
///  Device   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////

auto Device::surface() const noexcept -> const vk::raii::SurfaceKHR&
{
    return m_surface;
}

auto Device::physical_device() const noexcept -> const vk::raii::PhysicalDevice&
{
    return m_physical_device;
}

auto Device::device() const noexcept -> const vk::raii::Device&
{
    return m_device;
}

auto Device::graphics_queue() const noexcept -> const vk::raii::Queue&
{
    return m_graphics_queue;
}

auto Device::present_queue() const noexcept -> const vk::raii::Queue&
{
    return m_present_queue;
}

auto Device::graphics_queue_family() const noexcept -> uint32_t
{
    return m_graphics_queue_family;
}

auto Device::present_queue_family() const noexcept -> uint32_t
{
    return m_present_queue_family;
}

}   // namespace engine::renderer
