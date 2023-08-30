#include "Device.hpp"

#include <functional>
#include <ranges>

#include "vulkan/helpers.hpp"

const std::vector<const char*> g_device_extensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

namespace {

auto supports_surface(
    vk::PhysicalDevice t_physical_devices,
    vk::SurfaceKHR     t_surface
) noexcept -> bool
{
    uint32_t i{ 0 };
    for (auto prop : t_physical_devices.getQueueFamilyProperties()) {
        auto [result, supported]{
            t_physical_devices.getSurfaceSupportKHR(i, t_surface)
        };
        if (result != vk::Result::eSuccess) {
            return false;
        }
        if (prop.queueCount > 0 && supported) {
            return true;
        }
    }
    return false;
}

auto find_graphics_queue_family(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> std::optional<uint32_t>
{
    auto queue_families{ t_physical_device.getQueueFamilyProperties() };
    for (uint32_t i{}; i < queue_families.size(); i++) {
        if (auto [result, success]{
                t_physical_device.getSurfaceSupportKHR(i, t_surface) };
            result == vk::Result::eSuccess && success
            && queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            return i;
        }
    }

    return std::nullopt;
}

}   // namespace

namespace engine::renderer {

///////////////////////////////////////
///---------------------------------///
///  Device   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////

auto Device::create(
    vk::Instance       t_instance,
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device,
    const Config&      t_config
) noexcept -> std::optional<Device>
{
    if (!t_instance || !t_surface || !t_physical_device
        || !adequate(t_physical_device, t_surface)
        || !vulkan::supports_extensions(t_physical_device, t_config.extensions))
    {
        return std::nullopt;
    }

    auto graphics_queue_family_index{
        find_graphics_queue_family(t_physical_device, t_surface)
    };
    if (!graphics_queue_family_index.has_value()) {
        return std::nullopt;
    }

    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    {
        auto queue_priority{ 1.f };

        queue_create_infos.push_back(vk::DeviceQueueCreateInfo{
            .queueFamilyIndex = *graphics_queue_family_index,
            .queueCount       = 1,
            .pQueuePriorities = &queue_priority });
    }

    std::vector<const char*> extensions{ g_device_extensions };
    for (auto extension : t_config.extensions) {
        extensions.push_back(extension);
    }

    auto [result, device]{ t_physical_device.createDevice(vk::DeviceCreateInfo{
        .pNext = t_config.next,
        .queueCreateInfoCount =
            static_cast<uint32_t>(queue_create_infos.size()),
        .pQueueCreateInfos       = queue_create_infos.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures        = &t_config.features }) };
    if (result != vk::Result::eSuccess) {
        return std::nullopt;
    }

    return Device{ t_physical_device,
                   device,
                   *graphics_queue_family_index,
                   device.getQueue(*graphics_queue_family_index, 0),
                   0,
                   nullptr,
                   0,
                   nullptr };
}

auto Device::adequate(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool
{
    return vulkan::supports_extensions(t_physical_device, g_device_extensions)
        && supports_surface(t_physical_device, t_surface);
}

Device::Device(
    vk::PhysicalDevice t_physical_device,
    vk::Device         t_device,
    uint32_t           t_graphics_family_index,
    vk::Queue          t_graphics_queue,
    uint32_t           t_compute_queue_family_index,
    vk::Queue          t_compute_queue,
    uint32_t           t_transfer_queue_family_index,
    vk::Queue          t_transfer_queue
) noexcept
    : m_physical_device{ t_physical_device },
      m_device{ t_device },
      m_graphics_queue_family_index{ t_graphics_family_index },
      m_graphics_queue{ t_graphics_queue },
      m_compute_queue_family_index{ t_compute_queue_family_index },
      m_compute_queue{ t_compute_queue },
      m_transfer_queue_family_index{ t_transfer_queue_family_index },
      m_transfer_queue{ t_transfer_queue }
{}

auto Device::operator*() const noexcept -> vk::Device
{
    return *m_device;
}

auto Device::operator->() const noexcept -> const vk::Device*
{
    return m_device.operator->();
}

auto Device::physical_device() const noexcept -> vk::PhysicalDevice
{
    return m_physical_device;
}

auto Device::graphics_queue_family_index() const noexcept -> uint32_t
{
    return m_graphics_queue_family_index;
}

auto Device::graphics_queue() const noexcept -> vk::Queue
{
    return m_graphics_queue;
}

}   // namespace engine::renderer
