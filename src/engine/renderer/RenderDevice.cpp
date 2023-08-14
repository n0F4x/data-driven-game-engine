#include "RenderDevice.hpp"

#include <functional>
#include <ranges>
#include <set>
#include <string_view>

#include "engine/utility/vulkan/helpers.hpp"

namespace {

const std::vector<const char*> g_device_extensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
};

auto supports_extensions(
    vk::PhysicalDevice           t_physical_device,
    std::span<const char* const> t_device_extensions
) noexcept -> bool
{
    auto [result, extension_properties]{
        t_physical_device.enumerateDeviceExtensionProperties()
    };
    if (result != vk::Result::eSuccess) {
        return false;
    }

    std::set<std::string_view> required_extensions{ t_device_extensions.begin(),
                                                    t_device_extensions.end() };

    for (const auto& extension : extension_properties) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

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

auto choose_physical_device(
    std::span<vk::PhysicalDevice> t_physical_devices,
    std::span<const char* const>  t_required_extensions,
    vk::SurfaceKHR                t_surface
) noexcept -> std::optional<vk::PhysicalDevice>
{
    if (t_physical_devices.empty()) {
        return std::nullopt;
    }

    std::optional<vk::PhysicalDevice> fallback_device;
    std::optional<vk::PhysicalDevice> fallback_integrated_device;
    for (auto physical_device : t_physical_devices) {
        if (!(supports_extensions(physical_device, t_required_extensions)
              && supports_surface(physical_device, t_surface)))
        {
            continue;
        }

        auto device_type{ physical_device.getProperties().deviceType };

        if (device_type == vk::PhysicalDeviceType::eDiscreteGpu) {
            return physical_device;
        }
        else if (device_type == vk::PhysicalDeviceType::eIntegratedGpu) {
            fallback_integrated_device = physical_device;
        }
        else {
            fallback_device = physical_device;
        }
    }

    if (fallback_integrated_device.has_value()) {
        return fallback_integrated_device;
    }
    return fallback_device;
}

struct QueueFamilyIndices {
    uint32_t graphics_family;
    uint32_t present_family;
};

auto find_queue_families(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> std::optional<QueueFamilyIndices>
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

        if (auto [result, success]{
                t_physical_device.getSurfaceSupportKHR(i, t_surface) };
            result == vk::Result::eSuccess && success)
        {
            present_family = i;
        }

        if (graphics_family && present_family
            && *graphics_family == *present_family)
        {
            return QueueFamilyIndices{ .graphics_family = *graphics_family,
                                       .present_family  = *present_family };
        }

        i++;
    }

    if (graphics_family && present_family) {
        return QueueFamilyIndices{ .graphics_family = *graphics_family,
                                   .present_family  = *present_family };
    }

    return std::nullopt;
}

}   // namespace

namespace engine::renderer {

///////////////////////////////////////
///---------------------------------///
///  RenderDevice   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////

RenderDevice::RenderDevice(
    vulkan::Instance&&                         t_instance,
    std::optional<vulkan::DebugUtilsMessenger> t_messenger,
    vk::PhysicalDevice                         t_physical_device,
    uint32_t                                   t_graphics_family_index,
    uint32_t                                   t_present_family_index,
    vulkan::Device&&                           t_device
) noexcept
    : m_instance{ std::move(t_instance) },
      m_messenger{ std::move(t_messenger) },
      m_physical_device{ t_physical_device },
      m_graphics_family_index{ t_graphics_family_index },
      m_present_family_index{ t_present_family_index },
      m_device{ std::move(t_device) }
{}

auto RenderDevice::operator*() const noexcept -> vk::Device
{
    return *m_device;
}

auto RenderDevice::operator->() const noexcept -> const vk::Device*
{
    return m_device.operator->();
}

auto RenderDevice::physical_device() const noexcept -> vk::PhysicalDevice
{
    return m_physical_device;
}

auto RenderDevice::graphics_queue_family_index() const noexcept -> uint32_t
{
    return m_graphics_family_index;
}

auto RenderDevice::present_queue_family_index() const noexcept -> uint32_t
{
    return m_present_family_index;
}

auto RenderDevice::create(
    vulkan::Instance&&                         t_instance,
    std::optional<vulkan::DebugUtilsMessenger> t_messenger,
    vk::SurfaceKHR                             t_surface
) noexcept -> std::optional<RenderDevice>
{
    auto physical_devices{ t_instance->enumeratePhysicalDevices() };
    if (physical_devices.result != vk::Result::eSuccess) {
        return std::nullopt;
    }

    auto physical_device{ choose_physical_device(
        physical_devices.value, g_device_extensions, t_surface
    ) };
    if (!physical_device.has_value()) {
        return std::nullopt;
    }

    auto queue_family_indices{
        find_queue_families(*physical_device, t_surface)
    };
    if (!queue_family_indices.has_value()) {
        return std::nullopt;
    }
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    {
        auto queue_priority{ 1.f };

        for (auto queue_family :
             std::set{ queue_family_indices->graphics_family,
                       queue_family_indices->present_family })
        {
            queue_create_infos.push_back(vk::DeviceQueueCreateInfo{
                .queueFamilyIndex = queue_family,
                .queueCount       = 1,
                .pQueuePriorities = &queue_priority });
        }
    }
    vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature{
        .dynamicRendering = true,
    };
    auto device{ vulkan::Device::create()
                     .set_physical_device(*physical_device)
                     .add_queue_create_infos(queue_create_infos)
                     .add_enabled_layers(vulkan::validation_layers())
                     .add_enabled_extensions(g_device_extensions)
                     .build(&dynamic_rendering_feature) };
    if (!device.has_value()) {
        return std::nullopt;
    }

    return RenderDevice{ std::move(t_instance),
                         std::move(t_messenger),
                         *physical_device,
                         queue_family_indices->graphics_family,
                         queue_family_indices->present_family,
                         std::move(*device) };
}

}   // namespace engine::renderer
