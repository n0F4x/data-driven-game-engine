#include "Device.hpp"

#include <ranges>
#include <system_error>

#include <spdlog/spdlog.h>

#include "helpers.hpp"

using namespace core::renderer;

[[nodiscard]] static auto get_queue_infos(
    const vk::SurfaceKHR     t_surface,
    const vk::PhysicalDevice t_physical_device
) -> helpers::QueueInfos
{
    const auto queue_infos{ helpers::find_queue_families(t_physical_device, t_surface) };
    if (!queue_infos.has_value()) {
        throw std::system_error{ std::error_code{},
                                 "core::renderer::Device could not find appropriate "
                                 "Vulkan queues" };
    }
    return queue_infos.value();
}

[[nodiscard]] static auto create_device(
    const vk::PhysicalDevice   t_physical_device,
    const Device::CreateInfo&  t_create_info,
    const helpers::QueueInfos& t_queue_infos
) -> vk::UniqueDevice
{
    const auto enabled_extension_names{ t_create_info.extensions
                                        | std::views::transform(&std::string::c_str)
                                        | std::ranges::to<std::vector>() };

    const vk::DeviceCreateInfo device_create_info{
        .pNext = t_create_info.next,
        .queueCreateInfoCount =
            static_cast<uint32_t>(t_queue_infos.queue_create_infos.size()),
        .pQueueCreateInfos       = t_queue_infos.queue_create_infos.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(enabled_extension_names.size()),
        .ppEnabledExtensionNames = enabled_extension_names.data(),
        .pEnabledFeatures        = &t_create_info.features
    };

    return t_physical_device.createDeviceUnique(device_create_info);
}

namespace core::renderer {

///////////////////////////////////////
///---------------------------------///
///  Device   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////

Device::Device(
    const vk::SurfaceKHR     t_surface,
    const vk::PhysicalDevice t_physical_device,
    const CreateInfo&        t_create_info
)
    : Device{ t_physical_device,
              t_create_info,
              get_queue_infos(t_surface, t_physical_device) }
{}

auto Device::operator*() const noexcept -> vk::Device
{
    return *m_device;
}

auto Device::operator->() const noexcept -> const vk::Device*
{
    return m_device.operator->();
}

auto Device::get() const noexcept -> vk::Device
{
    return *m_device;
}

auto Device::physical_device() const noexcept -> vk::PhysicalDevice
{
    return m_physical_device;
}

auto Device::enabled_extensions() const noexcept -> std::span<const std::string>
{
    return m_extensions;
}

auto Device::graphics_queue_family_index() const noexcept -> uint32_t
{
    return m_graphics_queue_family_index;
}

auto Device::graphics_queue() const noexcept -> vk::Queue
{
    return m_graphics_queue;
}

auto Device::compute_queue_family_index() const noexcept -> uint32_t
{
    return m_compute_queue_family_index;
}

auto Device::compute_queue() const noexcept -> vk::Queue
{
    return m_compute_queue;
}

auto Device::transfer_queue_family_index() const noexcept -> uint32_t
{
    return m_transfer_queue_family_index;
}

auto Device::transfer_queue() const noexcept -> vk::Queue
{
    return m_transfer_queue;
}

Device::Device(
    const vk::PhysicalDevice   t_physical_device,
    const Device::CreateInfo&  t_create_info,
    const helpers::QueueInfos& t_queue_infos
)
    : Device{ t_physical_device,
              t_create_info,
              t_queue_infos,
              create_device(t_physical_device, t_create_info, t_queue_infos) }
{}

Device::Device(
    const vk::PhysicalDevice   t_physical_device,
    const CreateInfo&          t_create_info,
    const helpers::QueueInfos& t_queue_infos,
    vk::UniqueDevice&&         t_device
)
    : Device{
          t_physical_device,
          t_create_info,
          std::move(t_device),
          t_queue_infos.graphics_family,
          t_device->getQueue(t_queue_infos.graphics_family, t_queue_infos.graphics_index),
          t_queue_infos.compute_family,
          t_device->getQueue(t_queue_infos.compute_family, t_queue_infos.compute_index),
          t_queue_infos.transfer_family,
          t_device->getQueue(t_queue_infos.transfer_family, t_queue_infos.transfer_index)
      }
{}

Device::Device(
    const vk::PhysicalDevice t_physical_device,
    const CreateInfo&        t_create_info,
    vk::UniqueDevice&&       t_device,
    const uint32_t           t_graphics_family_index,
    const vk::Queue          t_graphics_queue,
    const uint32_t           t_compute_queue_family_index,
    const vk::Queue          t_compute_queue,
    const uint32_t           t_transfer_queue_family_index,
    const vk::Queue          t_transfer_queue
) noexcept
    : m_physical_device{ t_physical_device },
      m_extensions{ std::from_range, t_create_info.extensions },
      m_device{ std::move(t_device) },
      m_graphics_queue_family_index{ t_graphics_family_index },
      m_graphics_queue{ t_graphics_queue },
      m_compute_queue_family_index{ t_compute_queue_family_index },
      m_compute_queue{ t_compute_queue },
      m_transfer_queue_family_index{ t_transfer_queue_family_index },
      m_transfer_queue{ t_transfer_queue }
{
    const auto properties{ t_physical_device.getProperties() };

    std::string enabled_extensions{ "\nEnabled device extensions:" };
    for (const auto& extension : m_extensions) {
        enabled_extensions += '\n';
        enabled_extensions += '\t';
        enabled_extensions += extension;
    }

    SPDLOG_INFO(
        "Found GPU({}) with Vulkan version: {}.{}.{}",
        properties.deviceName.operator std::string(),
        VK_VERSION_MAJOR(properties.apiVersion),
        VK_VERSION_MINOR(properties.apiVersion),
        VK_VERSION_PATCH(properties.apiVersion)
    );

    SPDLOG_DEBUG(enabled_extensions);
}

}   // namespace core::renderer
