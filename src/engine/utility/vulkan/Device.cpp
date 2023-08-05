#include "Device.hpp"

namespace engine::vulkan {

/////////////////////////////////
///---------------------------///
///  Device   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////

Device::Device(vk::Device t_device) noexcept : m_device{ t_device } {}

Device::Device(Device&& t_other) noexcept : m_device{ t_other.m_device }
{
    t_other.m_device = nullptr;
}

Device::~Device() noexcept
{
    if (m_device) {
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

auto Device::create() noexcept -> Builder
{
    return Builder{};
}

//////////////////////////////////////////
///------------------------------------///
///  Device::Builder   IMPLEMENTATION  ///
///------------------------------------///
//////////////////////////////////////////

auto Device::Builder::build(const void* t_next) noexcept
    -> std::expected<Device, vk::Result>
{
    if (!m_physical_device) {
        return std::unexpected{ vk::Result::eErrorInitializationFailed };
    }

    auto [result, device]{ m_physical_device.createDevice(vk::DeviceCreateInfo{
        .pNext = t_next,
        .queueCreateInfoCount =
            static_cast<uint32_t>(m_queue_create_infos.size()),
        .pQueueCreateInfos   = m_queue_create_infos.data(),
        .enabledLayerCount   = static_cast<uint32_t>(m_enabled_layers.size()),
        .ppEnabledLayerNames = m_enabled_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(m_enabled_extensions.size()),
        .ppEnabledExtensionNames = m_enabled_extensions.data(),
        .pEnabledFeatures        = &m_enabled_features }) };
    if (result == vk::Result::eSuccess) {
        return Device{ device };
    }
    else {
        return std::unexpected{ result };
    }
}

auto Device::Builder::set_physical_device(vk::PhysicalDevice t_physical_device
) noexcept -> Builder&
{
    m_physical_device = t_physical_device;
    return *this;
}

auto Device::Builder::add_queue_create_infos(
    std::span<const vk::DeviceQueueCreateInfo> t_queue_create_info
) noexcept -> Builder&
{
    m_queue_create_infos.insert(
        m_queue_create_infos.end(),
        t_queue_create_info.begin(),
        t_queue_create_info.end()
    );
    return *this;
}

auto Device::Builder::add_enabled_layers(
    std::span<const char* const> t_enabled_layers
) noexcept -> Builder&
{
    m_enabled_layers.insert(
        m_enabled_layers.end(), t_enabled_layers.begin(), t_enabled_layers.end()
    );
    return *this;
}

auto Device::Builder::add_enabled_extensions(
    std::span<const char* const> t_enabled_extensions
) noexcept -> Builder&
{
    m_enabled_extensions.insert(
        m_enabled_extensions.end(),
        t_enabled_extensions.begin(),
        t_enabled_extensions.end()
    );
    return *this;
}

auto Device::Builder::set_enabled_features(
    vk::PhysicalDeviceFeatures t_enabled_features
) noexcept -> Builder&
{
    m_enabled_features = t_enabled_features;
    return *this;
}

}   // namespace engine::vulkan
