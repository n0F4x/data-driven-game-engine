#include "Device.hpp"

namespace engine::renderer::vulkan {

Device::Device(vk::Device t_device) noexcept : m_device{ t_device } {}

Device::Device(Device&& t_other) noexcept : Device{ t_other.m_device }
{
    t_other.m_device = nullptr;
}

Device::~Device() noexcept
{
    if (m_device) {
        static_cast<void>(m_device.waitIdle());
        m_device.destroy();
    }
}

auto Device::operator*() const noexcept -> vk::Device
{
    return m_device;
}

auto Device::operator->() const noexcept -> const vk::Device*
{
    return &m_device;
}

}   // namespace engine::renderer::vulkan
