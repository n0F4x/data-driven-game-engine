#include "Device.hpp"

#include "engine/utility/vulkan/tools.hpp"

#include "helpers.hpp"

namespace engine::renderer {

///////////////////////////////////////
///---------------------------------///
///  Device   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////

auto Device::create(
    const Instance&    t_instance,
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device,
    const CreateInfo&  t_create_info
) noexcept -> std::optional<Device>
{
    if (!t_surface || !t_physical_device
        || !helpers::is_adequate(t_physical_device, t_surface))
    {
        return std::nullopt;
    }

    const auto queue_infos{
        helpers::find_queue_families(t_physical_device, t_surface)
    };
    if (!queue_infos.has_value()) {
        return std::nullopt;
    }

    const auto [result, device]{
        t_physical_device.createDevice(vk::DeviceCreateInfo{
            .pNext = t_create_info.next,
            .queueCreateInfoCount =
                static_cast<uint32_t>(queue_infos->queue_create_infos.size()),
            .pQueueCreateInfos = queue_infos->queue_create_infos.data(),
            .enabledExtensionCount =
                static_cast<uint32_t>(t_create_info.extensions.size()),
            .ppEnabledExtensionNames = t_create_info.extensions.data(),
            .pEnabledFeatures        = &t_create_info.features })
    };
    if (result != vk::Result::eSuccess) {
        SPDLOG_ERROR(
            "vk::PhysicalDevice::createDevice failed with error code {}",
            std::to_underlying(result)
        );
        return std::nullopt;
    }

    const VmaVulkanFunctions vulkan_functions = { .vkGetInstanceProcAddr =
                                                      &vkGetInstanceProcAddr,
                                                  .vkGetDeviceProcAddr =
                                                      &vkGetDeviceProcAddr };

    const VmaAllocatorCreateInfo allocator_info{
        .flags = helpers::vma_allocator_create_flags(
            t_instance.enabled_extensions(), t_create_info.extensions
        ),
        .physicalDevice   = t_physical_device,
        .device           = device,
        .pVulkanFunctions = &vulkan_functions,
        .instance         = *t_instance,
    };
    VmaAllocator allocator;
    vmaCreateAllocator(&allocator_info, &allocator);

    return Device{ t_physical_device,
                   t_create_info,
                   device,
                   allocator,
                   queue_infos->graphics_family,
                   device.getQueue(
                       queue_infos->graphics_family, queue_infos->graphics_index
                   ),
                   queue_infos->compute_family,
                   device.getQueue(
                       queue_infos->compute_family, queue_infos->compute_index
                   ),
                   queue_infos->transfer_family,
                   device.getQueue(
                       queue_infos->transfer_family, queue_infos->transfer_index
                   ) };
}

auto Device::create_default(
    const Instance&    t_instance,
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device
) noexcept -> std::optional<Device>
{
    return create(
        t_instance,
        t_surface,
        t_physical_device,
        CreateInfo{ .extensions =
                        helpers::device_extensions(t_physical_device) }
    );
}

Device::Device(
    vk::PhysicalDevice t_physical_device,
    const CreateInfo&  t_info,
    vk::Device         t_device,
    VmaAllocator       t_allocator,
    uint32_t           t_graphics_family_index,
    vk::Queue          t_graphics_queue,
    uint32_t           t_compute_queue_family_index,
    vk::Queue          t_compute_queue,
    uint32_t           t_transfer_queue_family_index,
    vk::Queue          t_transfer_queue
) noexcept
    : m_physical_device{ t_physical_device },
      m_info{ t_info },
      m_device{ t_device },
      m_allocator{ t_allocator },
      m_graphics_queue_family_index{ t_graphics_family_index },
      m_graphics_queue{ t_graphics_queue },
      m_compute_queue_family_index{ t_compute_queue_family_index },
      m_compute_queue{ t_compute_queue },
      m_transfer_queue_family_index{ t_transfer_queue_family_index },
      m_transfer_queue{ t_transfer_queue }
{
    const auto properties{ t_physical_device.getProperties() };

    std::string enabled_extensions{ "\nEnabled device extensions:" };
    for (auto extension : m_info.extensions) {
        enabled_extensions += '\n';
        enabled_extensions += '\t';
        enabled_extensions += extension;
    }

    SPDLOG_INFO(
        "Found GPU({}) with Vulkan version: {}.{}.{}",
        properties.deviceName,
        VK_VERSION_MAJOR(properties.apiVersion),
        VK_VERSION_MINOR(properties.apiVersion),
        VK_VERSION_PATCH(properties.apiVersion)
    );

    SPDLOG_TRACE(enabled_extensions);
}

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

auto Device::allocator() const noexcept -> VmaAllocator
{
    return *m_allocator;
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

}   // namespace engine::renderer
