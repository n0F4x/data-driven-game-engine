module;


#include <VkBootstrap.h>

module modules.renderer.base.device.Device;

namespace modules::renderer::base {

Device::Device(const vkb::Device& device) : m_device{ device } {}

Device::Device(vkb::Device&& device) noexcept : m_device{ std::move(device) } {}

Device::Device(Device&& other) noexcept : m_device{ std::exchange(other.m_device, {}) } {}

Device::~Device()
{
    if (m_device.device != nullptr) {
        vkb::destroy_device(m_device);
    }
}

auto Device::operator*() const noexcept -> vk::Device
{
    return m_device.device;
}

auto Device::operator->() const noexcept -> const vk::Device*
{
    return reinterpret_cast<const vk::Device*>(&m_device.device);
}

Device::operator vkb::Device() const
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

auto Device::info() const -> vkb::Device
{
    return m_device;
}

}   // namespace modules::renderer::base
