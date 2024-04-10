#include "Device.hpp"

namespace core::renderer {

Device::Device(const vkb::Device& t_device) noexcept : m_device{ t_device } {}

Device::Device(vkb::Device&& t_device) noexcept : m_device{ std::move(t_device) } {}

Device::~Device()
{
    vkb::destroy_device(m_device);
}

auto Device::operator*() const noexcept -> vk::Device
{
    return m_device.device;
}

auto Device::operator->() const noexcept -> const vk::Device*
{
    return reinterpret_cast<const vk::Device*>(&m_device.device);
}

Device::operator vkb::Device() const noexcept
{
    return m_device;
}

auto Device::get() const noexcept -> vk::Device
{
    return m_device.device;
}

auto Device::physical_device() const noexcept -> vk::PhysicalDevice
{
    return m_device.physical_device.physical_device;
}

auto Device::info() const noexcept -> vkb::Device
{
    return m_device;
}

}   // namespace core::renderer
