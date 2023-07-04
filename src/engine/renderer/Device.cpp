#include "Device.hpp"

#include <set>
#include <string_view>

#include "engine/core/vulkan.hpp"

namespace {

const std::vector g_device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

struct QueueFamilyIndices {
    uint32_t graphics_family;
    uint32_t present_family;
};

std::optional<QueueFamilyIndices> find_queue_families(
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device
)
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    uint32_t i = 0;
    for (const auto& queue_family :
         t_physical_device.getQueueFamilyProperties())
    {
        if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {
            graphics_family = i;
        }

        if (t_physical_device.getSurfaceSupportKHR(i, t_surface)) {
            present_family = i;
        }

        if (graphics_family && present_family
            && graphics_family == present_family)
        {
            return QueueFamilyIndices{
                .graphics_family = graphics_family.value(),
                .present_family  = present_family.value()
            };
        }

        i++;
    }

    if (graphics_family && present_family) {
        return QueueFamilyIndices{ .graphics_family = graphics_family.value(),
                                   .present_family  = present_family.value() };
    }

    return std::nullopt;
}

bool supports_extensions(
    vk::PhysicalDevice           t_physical_device,
    std::span<const char* const> t_device_extensions
)
{
    auto extension_properties{
        t_physical_device.enumerateDeviceExtensionProperties()
    };

    std::set<std::string_view> required_extensions{ t_device_extensions.begin(),
                                                    t_device_extensions.end() };

    for (const auto& extension : extension_properties) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

bool suitable(
    vk::PhysicalDevice          t_physical_device,
    const vk::raii::SurfaceKHR& t_surface
)
{
    if (!find_queue_families(*t_surface, t_physical_device).has_value()) {
        return false;
    }

    if (!supports_extensions(t_physical_device, g_device_extensions)) {
        return false;
    }

    if (t_physical_device.getSurfaceFormatsKHR(*t_surface).empty()
        || t_physical_device.getSurfacePresentModesKHR(*t_surface).empty())
    {
        return false;
    }

    return true;
}

vk::raii::PhysicalDevice pick_physical_device(
    const vk::raii::Instance&   t_instance,
    const vk::raii::SurfaceKHR& t_surface
)
{
    vk::raii::PhysicalDevices physical_devices{ t_instance };

    if (physical_devices.empty()) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    std::vector<size_t> suitable_indices;
    for (size_t i = 0; i < physical_devices.size(); i++) {
        if (suitable(*physical_devices[i], t_surface)) {
            if (physical_devices[i].getProperties().deviceType
                == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return std::move(physical_devices[i]);
            }
            suitable_indices.push_back(i);
        }
    }

    if (!suitable_indices.empty()) {
        return std::move(physical_devices[suitable_indices.front()]);
    }

    throw std::runtime_error("Failed to find a suitable GPU");
}

vk::raii::Device create_device(
    const vk::raii::SurfaceKHR&     t_surface,
    const vk::raii::PhysicalDevice& t_physical_device
)
{
    QueueFamilyIndices queue_family_indices =
        find_queue_families(*t_surface, *t_physical_device).value();
    float                                  queue_priority = 1.f;
    std::vector<vk::DeviceQueueCreateInfo> device_queue_create_infos;

    for (auto queue_family : std::set{ queue_family_indices.graphics_family,
                                       queue_family_indices.present_family })
    {
        device_queue_create_infos.push_back({ .queueFamilyIndex = queue_family,
                                              .queueCount       = 1,
                                              .pQueuePriorities =
                                                  &queue_priority });
    }

    vk::PhysicalDeviceFeatures device_features{};
    auto validation_layers{ engine::utils::create_validation_layers() };

    vk::DeviceCreateInfo device_create_info{
        .queueCreateInfoCount =
            static_cast<uint32_t>(device_queue_create_infos.size()),
        .pQueueCreateInfos   = device_queue_create_infos.data(),
        .enabledLayerCount   = static_cast<uint32_t>(validation_layers.size()),
        .ppEnabledLayerNames = validation_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(g_device_extensions.size()),
        .ppEnabledExtensionNames = g_device_extensions.data(),
        .pEnabledFeatures        = &device_features
    };

    return vk::raii::Device{ t_physical_device, device_create_info };
}

}   // namespace

namespace engine::renderer {

/////////////////////////////////
///---------------------------///
///  Device   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////
Device::Device(
    const vk::raii::Instance&   t_instance,
    const vk::raii::SurfaceKHR& t_surface
)
    : m_physical_device{ pick_physical_device(t_instance, t_surface) },
      m_device{ create_device(t_surface, m_physical_device) },
      m_graphics_queue_family{
          find_queue_families(*t_surface, *m_physical_device)
              .value()
              .graphics_family
      },
      m_present_queue_family{ find_queue_families(*t_surface, *m_physical_device)
                                  .value()
                                  .present_family },
      m_graphics_queue{ m_device, m_graphics_queue_family, 0 },
      m_present_queue{ m_device, m_present_queue_family, 0 }
{}

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
