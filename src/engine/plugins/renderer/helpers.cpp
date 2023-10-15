#include "helpers.hpp"

#include <iostream>
#include <ranges>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#if defined(_WIN32)
// clang-format off
  #include <windows.h>
  #include <vulkan/vulkan_win32.h>
// clang-format on
#elif defined(__linux__)
  #include <vulkan/vulkan_xcb.h>
#elif defined(__ANDROID__)
  #include <vulkan/vulkan_android.h>
#endif

#include <vulkan/vulkan_to_string.hpp>

#include "engine/utility/vulkan/tools.hpp"

namespace {

const uint32_t g_api_version{ VK_API_VERSION_1_3 };

const std::vector<const char*> g_required_layers{};
const std::vector<const char*> g_optional_layers{
#ifdef ENGINE_VULKAN_DEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
};

const std::vector<const char*> g_required_instance_extensions
{
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#else
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
};
const std::vector<const char*> g_optional_instance_extensions{
#ifdef ENGINE_VULKAN_DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
};

const std::vector<const char*> g_required_device_extensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
const std::vector<const char*> g_optional_device_extensions{
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME
};

[[nodiscard]] auto filter(
    std::span<const char* const> t_available,
    std::span<const char* const> t_required,
    std::span<const char* const> t_optional
) noexcept -> std::vector<const char*>
{
    std::vector<const char*> filtered{ t_required.cbegin(), t_required.cend() };

    for (const auto optional : t_optional) {
        if (std::ranges::any_of(t_available, [optional](const auto available) {
                return strcmp(optional, available);
            }))
        {
            filtered.emplace_back(optional);
        }
    }

    return filtered;
}

auto find_graphics_queue_family(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> std::optional<uint32_t>
{
    for (const auto [index, properties] :
         std::views::enumerate(t_physical_device.getQueueFamilyProperties()))
    {
        if (const auto [result, success]{
                t_physical_device.getSurfaceSupportKHR(
                    static_cast<uint32_t>(index), t_surface
                ) };
            result == vk::Result::eSuccess && success
            && properties.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            return static_cast<uint32_t>(index);
        }
        else if (result != vk::Result::eSuccess) {
            SPDLOG_ERROR(
                "vk::PhysicalDevice::getQueueFamilyProperties failed with "
                "error code {}",
                std::to_underlying(result)
            );
        }
    }

    return std::nullopt;
}

[[nodiscard]] auto find_transfer_queue_family(
    vk::PhysicalDevice t_physical_device,
    uint32_t           t_graphics_queue_family
) -> std::optional<uint32_t>
{
    const auto queue_families{ t_physical_device.getQueueFamilyProperties() };

    for (const auto [index, properties] : std::views::enumerate(queue_families))
    {
        if (!(properties.queueFlags & vk::QueueFlagBits::eGraphics)
            && !(properties.queueFlags & vk::QueueFlagBits::eCompute)
            && properties.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            return static_cast<uint32_t>(index);
        }
    }

    for (const auto [index, properties] : std::views::enumerate(queue_families))
    {
        if (static_cast<uint32_t>(index) == t_graphics_queue_family
            && properties.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            return static_cast<uint32_t>(index);
        }
    }

    for (const auto [index, properties] : std::views::enumerate(queue_families))
    {
        if (properties.queueFlags & vk::QueueFlagBits::eTransfer) {
            return static_cast<uint32_t>(index);
        }
    }

    return std::nullopt;
}

auto find_compute_queue_family(
    vk::PhysicalDevice t_physical_device,
    uint32_t           t_graphics_queue_family
) noexcept -> std::optional<uint32_t>
{
    const auto queue_families{ t_physical_device.getQueueFamilyProperties() };

    for (const auto [index, properties] : std::views::enumerate(queue_families))
    {
        if (!(properties.queueFlags & vk::QueueFlagBits::eGraphics)
            && properties.queueFlags & vk::QueueFlagBits::eCompute)
        {
            return static_cast<uint32_t>(index);
        }
    }

    for (const auto [index, properties] : std::views::enumerate(queue_families))
    {
        if (static_cast<uint32_t>(index) != t_graphics_queue_family
            && properties.queueFlags & vk::QueueFlagBits::eCompute)
        {
            return static_cast<uint32_t>(index);
        }
    }

    for (const auto [index, properties] : std::views::enumerate(queue_families))
    {
        if (properties.queueFlags & vk::QueueFlagBits::eCompute) {
            return static_cast<uint32_t>(index);
        }
    }

    return std::nullopt;
}

}   // namespace

namespace engine::renderer::helpers {

auto application_info() noexcept -> const vk::ApplicationInfo&
{
    static const vk::ApplicationInfo application_create_info{
        .apiVersion = g_api_version
    };
    return application_create_info;
}

auto layers() noexcept -> std::span<const char* const>
{
    static const std::vector<const char*> layers{ []() {
        auto                              expected_available_layers{ utils::vulkan::available_layers() };
        if (!expected_available_layers.has_value()) {
            return g_required_layers;
        }
        return filter(
            *expected_available_layers, g_required_layers, g_optional_layers
        );
    }() };

    return layers;
}

auto instance_extensions() noexcept -> std::span<const char* const>
{
    static const std::vector<const char*> instance_extensions{ []() {
        auto                              expected_available_instance_extensions{
            utils::vulkan::available_instance_extensions()
        };
        if (!expected_available_instance_extensions.has_value()) {
            return g_required_instance_extensions;
        }
        return filter(
            *expected_available_instance_extensions,
            g_required_instance_extensions,
            g_optional_instance_extensions
        );
    }() };

    return instance_extensions;
}

}   // namespace engine::renderer::helpers

PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance                                instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks*              pAllocator,
    VkDebugUtilsMessengerEXT*                 pMessenger
)
{
    return pfnVkCreateDebugUtilsMessengerEXT(
        instance, pCreateInfo, pAllocator, pMessenger
    );
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance                   instance,
    VkDebugUtilsMessengerEXT     messenger,
    const VkAllocationCallbacks* pAllocator
)
{
    return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /*pUserData*/
)
{
    std::ostringstream message;

    message << vk::to_string(
        static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)
    ) << ": "
            << vk::to_string(
                   static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)
               )
            << ":\n";
    message << std::string("\t") << "messageIDName   = <"
            << pCallbackData->pMessageIdName << ">\n";
    message << std::string("\t")
            << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    message << std::string("\t") << "message         = <"
            << pCallbackData->pMessage << ">\n";
    if (0 < pCallbackData->queueLabelCount) {
        message << std::string("\t") << "Queue Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++) {
            message << std::string("\t\t") << "labelName = <"
                    << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount) {
        message << std::string("\t") << "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
            message << std::string("\t\t") << "labelName = <"
                    << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->objectCount) {
        message << std::string("\t") << "Objects:\n";
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
            message << std::string("\t\t") << "Object " << i << "\n";
            message << std::string("\t\t\t") << "objectType   = "
                    << vk::to_string(static_cast<vk::ObjectType>(
                           pCallbackData->pObjects[i].objectType
                       ))
                    << "\n";
            message << std::string("\t\t\t") << "objectHandle = "
                    << pCallbackData->pObjects[i].objectHandle << "\n";
            if (pCallbackData->pObjects[i].pObjectName) {
                message << std::string("\t\t\t") << "objectName   = <"
                        << pCallbackData->pObjects[i].pObjectName << ">\n";
            }
        }
    }

    std::cout << message.str() << std::endl;

    return false;
}

namespace engine::renderer::helpers {

auto create_debug_messenger(vk::Instance t_instance) noexcept
    -> vk::DebugUtilsMessengerEXT
{
    const auto [result, properties] =
        vk::enumerateInstanceExtensionProperties();
    if (result != vk::Result::eSuccess) {
        SPDLOG_ERROR(
            "vk::enumerateInstanceExtensionProperties failed with error code "
            "{}",
            std::to_underlying(result)
        );
        return nullptr;
    }

    const auto propertyIterator = std::find_if(
        properties.cbegin(),
        properties.cend(),
        [](const vk::ExtensionProperties& ep) {
            return strcmp(ep.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
                == 0;
        }
    );
    if (propertyIterator == properties.cend()) {
        SPDLOG_ERROR(
            "{} Vulkan extension is not supported",
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        );
        return nullptr;
    }

    pfnVkCreateDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            t_instance.getProcAddr("vkCreateDebugUtilsMessengerEXT")
        );
    if (pfnVkCreateDebugUtilsMessengerEXT == nullptr) {
        SPDLOG_ERROR("pfnVkCreateDebugUtilsMessengerEXT not found");
        return nullptr;
    }

    pfnVkDestroyDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            t_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT")
        );
    if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr) {
        SPDLOG_ERROR("pfnVkDestroyDebugUtilsMessengerEXT not found");
        return nullptr;
    }

    const vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    const vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );
    const auto debugUtilsMessenger = t_instance.createDebugUtilsMessengerEXT(
        vk::DebugUtilsMessengerCreateInfoEXT{ .messageSeverity = severityFlags,
                                              .messageType = messageTypeFlags,
                                              .pfnUserCallback =
                                                  &debugMessageFunc }
    );
    if (debugUtilsMessenger.result != vk::Result::eSuccess) {
        SPDLOG_ERROR(
            "vk::Instance::createDebugUtilsMessengerEXT failed with error code "
            "{}",
            std::to_underlying(debugUtilsMessenger.result)
        );
        return nullptr;
    }

    return debugUtilsMessenger.value;
}

auto find_queue_families(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) -> std::optional<QueueInfos>
{
    const auto graphics_family{
        find_graphics_queue_family(t_physical_device, t_surface)
    };
    if (!graphics_family.has_value()) {
        return std::nullopt;
    }

    const auto compute_family{
        find_compute_queue_family(t_physical_device, *graphics_family)
    };
    if (!compute_family.has_value()) {
        return std::nullopt;
    }

    const auto transfer_family{
        find_transfer_queue_family(t_physical_device, *graphics_family)
    };
    if (!transfer_family.has_value()) {
        return std::nullopt;
    }

    QueueInfos queue_infos;
    queue_infos.graphics_family = *graphics_family;
    queue_infos.compute_family  = *compute_family;
    queue_infos.transfer_family = *transfer_family;

    auto queue_priority{ 1.f };

    queue_infos.queue_create_infos.push_back(vk::DeviceQueueCreateInfo{
        .queueFamilyIndex = *graphics_family,
        .queueCount       = 1,
        .pQueuePriorities = &queue_priority });
    queue_infos.graphics_index = 0;

    if (compute_family == graphics_family) {
        if (t_physical_device
                .getQueueFamilyProperties()[queue_infos.graphics_family]
                .queueCount
            > queue_infos.queue_create_infos[0].queueCount)
        {
            queue_infos.queue_create_infos[0].queueCount++;
        }
        queue_infos.compute_index =
            queue_infos.queue_create_infos[0].queueCount - 1;
    }
    else {
        queue_infos.queue_create_infos.push_back(vk::DeviceQueueCreateInfo{
            .queueFamilyIndex = *compute_family,
            .queueCount       = 1,
            .pQueuePriorities = &queue_priority });
        queue_infos.compute_index = 0;
    }

    if (transfer_family == graphics_family) {
        if (t_physical_device
                .getQueueFamilyProperties()[queue_infos.graphics_family]
                .queueCount
            > queue_infos.queue_create_infos[0].queueCount)
        {
            queue_infos.queue_create_infos[0].queueCount++;
        }
        queue_infos.transfer_index =
            queue_infos.queue_create_infos[0].queueCount - 1;
    }
    else if (transfer_family == compute_family) {
        if (t_physical_device
                .getQueueFamilyProperties()[queue_infos.compute_family]
                .queueCount
            > queue_infos.queue_create_infos[1].queueCount)
        {
            queue_infos.queue_create_infos[1].queueCount++;
        }
        queue_infos.transfer_index =
            queue_infos.queue_create_infos[1].queueCount - 1;
    }
    else {
        queue_infos.queue_create_infos.push_back(vk::DeviceQueueCreateInfo{
            .queueFamilyIndex = *transfer_family,
            .queueCount       = 1,
            .pQueuePriorities = &queue_priority });
        queue_infos.transfer_index = 0;
    }

    return queue_infos;
}

auto device_extensions(vk::PhysicalDevice t_physical_device) noexcept
    -> std::span<const char* const>
{
    static const std::vector<const char*> device_extensions{
        [t_physical_device]() {
            auto expected_available_device_extensions{
                utils::vulkan::available_device_extensions(t_physical_device)
            };
            if (!expected_available_device_extensions.has_value()) {
                return g_required_device_extensions;
            }
            return filter(
                *expected_available_device_extensions,
                g_required_device_extensions,
                g_optional_device_extensions
            );
        }()
    };

    return device_extensions;
}

auto is_adequate(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool
{
    return utils::vulkan::supports_surface(t_physical_device, t_surface)
        && utils::vulkan::supports_extensions(
               t_physical_device, device_extensions(t_physical_device)
        );
}

auto choose_physical_device(
    vk::Instance   t_instance,
    vk::SurfaceKHR t_surface
) noexcept -> vk::PhysicalDevice
{
    const auto [result, physical_devices]{ t_instance.enumeratePhysicalDevices(
    ) };
    if (result != vk::Result::eSuccess) {
        SPDLOG_ERROR(
            "vk::Instance::enumeratePhysicalDevices failed with error code {}",
            std::to_underlying(result)
        );
        return nullptr;
    }

    auto ranked_devices_view{
        physical_devices
        | std::views::filter(
            [t_surface](vk::PhysicalDevice t_physical_device) -> bool {
                return is_adequate(t_physical_device, t_surface);
            }
        )
        | std::views::transform(
            [](vk::PhysicalDevice t_physical_device
            ) -> std::pair<vk::PhysicalDevice, unsigned> {
                switch (t_physical_device.getProperties().deviceType) {
                    case vk::PhysicalDeviceType::eDiscreteGpu:
                        return std::make_pair(t_physical_device, 100);
                    case vk::PhysicalDeviceType::eIntegratedGpu:
                        return std::make_pair(t_physical_device, 10);
                    default: return std::make_pair(t_physical_device, 1);
                }
            }
        )
    };
    std::vector<std::pair<vk::PhysicalDevice, unsigned>> ranked_devices{
        ranked_devices_view.cbegin(), ranked_devices_view.cend()
    };
    std::ranges::sort(
        ranked_devices, {}, &std::pair<vk::PhysicalDevice, unsigned>::second
    );

    return ranked_devices.empty() ? nullptr : ranked_devices.front().first;
}

auto vma_allocator_create_flags(
    std::span<const char* const> enabled_instance_extensions,
    std::span<const char* const> enabled_device_extensions
) noexcept -> VmaAllocatorCreateFlags
{
    VmaAllocatorCreateFlags flags{};

    if (std::ranges::contains(
            enabled_device_extensions,
            VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME
        )
        && std::ranges::contains(
            enabled_device_extensions,
            VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    }

    if (std::ranges::contains(
            enabled_device_extensions, VK_KHR_BIND_MEMORY_2_EXTENSION_NAME
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;
    }

    if (std::ranges::contains(
            enabled_instance_extensions,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
        )
        && std::ranges::contains(
            enabled_device_extensions, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME
        ))
    {
        flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    }

    return flags;
}

}   // namespace engine::renderer::helpers