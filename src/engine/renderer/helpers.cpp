#include "helpers.hpp"

#include <cstring>
#include <iostream>
#include <ranges>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#if defined(_WIN32)
  #include <windows.h>

  #include <vulkan/vulkan_win32.h>
#elif defined(__linux__)
  #include <vulkan/vulkan_xcb.h>
#elif defined(__ANDROID__)
  #include <vulkan/vulkan_android.h>
#endif

#include <vulkan/vulkan_to_string.hpp>

#include "engine/utils/vulkan/tools.hpp"

namespace {

const uint32_t g_api_version{ VK_API_VERSION_1_3 };

const std::vector<std::string> g_required_layers{};
const std::vector<std::string> g_optional_layers{
#ifdef ENGINE_VULKAN_DEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
};

const std::vector<std::string> g_required_instance_extensions{
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#else
    VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
};
const std::vector<std::string> g_optional_instance_extensions{
#ifdef ENGINE_VULKAN_DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
};

const std::vector<std::string> g_required_device_extensions{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
const std::vector<std::string> g_optional_device_extensions{
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME
};

[[nodiscard]] auto filter(
    std::span<const std::string> t_available,
    std::span<const std::string> t_required,
    std::span<const std::string> t_optional
) noexcept -> std::vector<std::string>
{
    std::vector<std::string> filtered{ t_required.cbegin(), t_required.cend() };

    filtered.append_range(
        t_optional | std::views::filter([&](const auto& optional) {
            return std::ranges::contains(t_available, optional);
        })
    );

    return filtered;
}

auto find_graphics_queue_family(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> tl::optional<uint32_t>
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

    return tl::nullopt;
}

[[nodiscard]] auto find_transfer_queue_family(
    vk::PhysicalDevice t_physical_device,
    uint32_t           t_graphics_queue_family
) -> tl::optional<uint32_t>
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

    return tl::nullopt;
}

auto find_compute_queue_family(
    vk::PhysicalDevice t_physical_device,
    uint32_t           t_graphics_queue_family
) noexcept -> tl::optional<uint32_t>
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

    return tl::nullopt;
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

auto layers() noexcept -> std::span<const std::string>
{
    static const std::vector<std::string> layers{
        vulkan::available_layers()
            .transform([&](auto&& available_layers) {
                return filter(
                    available_layers, g_required_layers, g_optional_layers
                );
            })
            .value_or(g_required_layers)
    };

    return layers;
}

auto instance_extensions() noexcept -> std::span<const std::string>
{
    static const std::vector<std::string> instance_extensions{
        vulkan::available_instance_extensions()
            .transform([&](auto&& available_instance_extensions) {
                return filter(
                    available_instance_extensions,
                    g_required_instance_extensions,
                    g_optional_instance_extensions
                );
            })
            .value_or(g_required_instance_extensions)
    };

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
    auto extension_supported{ vulkan::available_instance_extensions()
                                  .transform([](auto extensions) {
                                      return std::ranges::contains(
                                          extensions,
                                          VK_EXT_DEBUG_UTILS_EXTENSION_NAME
                                      );
                                  })
                                  .value_or(false) };
    if (!extension_supported) {
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
) -> tl::optional<QueueInfos>
{
    const auto graphics_family{
        find_graphics_queue_family(t_physical_device, t_surface)
    };
    if (!graphics_family.has_value()) {
        return tl::nullopt;
    }

    const auto compute_family{
        find_compute_queue_family(t_physical_device, *graphics_family)
    };
    if (!compute_family.has_value()) {
        return tl::nullopt;
    }

    const auto transfer_family{
        find_transfer_queue_family(t_physical_device, *graphics_family)
    };
    if (!transfer_family.has_value()) {
        return tl::nullopt;
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
    -> std::span<const std::string>
{
    static const std::vector<std::string> device_extensions{
        vulkan::available_device_extensions(t_physical_device)
            .transform([](auto&& available_device_extensions) {
                return filter(
                    available_device_extensions,
                    g_required_device_extensions,
                    g_optional_device_extensions
                );
            })
            .value_or(g_required_device_extensions)
    };

    return device_extensions;
}

auto is_adequate(
    vk::PhysicalDevice t_physical_device,
    vk::SurfaceKHR     t_surface
) noexcept -> bool
{
    return vulkan::supports_surface(t_physical_device, t_surface)
        && vulkan::supports_extensions(
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

    auto adequate_devices{
        physical_devices
        | std::views::filter(
            [t_surface](vk::PhysicalDevice t_physical_device) -> bool {
                return is_adequate(t_physical_device, t_surface);
            }
        )
    };

    if (std::ranges::empty(adequate_devices)) {
        return nullptr;
    }

    auto ranked_devices{
        adequate_devices
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
        | std::ranges::to<std::vector>()
    };

    std::ranges::sort(
        ranked_devices,
        std::ranges::greater{},
        &std::pair<vk::PhysicalDevice, unsigned>::second
    );

    return ranked_devices.front().first;
}

auto vma_allocator_create_flags(
    std::span<const std::string> enabled_instance_extensions,
    std::span<const std::string> enabled_device_extensions
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
