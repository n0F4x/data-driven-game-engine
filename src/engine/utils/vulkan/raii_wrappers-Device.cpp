#include "raii_wrappers-Device.hpp"

#include <utility>

namespace engine::vulkan {

Device::Device(vk::Device t_device) noexcept : m_device{ t_device } {}

Device::Device(Device&& t_other) noexcept
    : Device{ std::exchange(t_other.m_device, nullptr) }
{}

Device::~Device() noexcept
{
    destroy();
}

auto Device::operator=(Device&& t_other) noexcept -> Device&
{
    if (this != &t_other) {
        destroy();

        std::swap(m_device, t_other.m_device);
    }
    return *this;
}

auto Device::operator*() const noexcept -> vk::Device
{
    return m_device;
}

auto Device::operator->() const noexcept -> const vk::Device*
{
    return &m_device;
}

auto Device::destroy() noexcept -> void
{
    if (m_device) {
        m_device.destroy();
    }
    m_device = nullptr;
}

}   // namespace engine::vulkan
