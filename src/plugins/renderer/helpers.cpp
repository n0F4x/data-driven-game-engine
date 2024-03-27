#include "helpers.hpp"

#include <spdlog/spdlog.h>

#include <vulkan/vulkan_to_string.hpp>

#include "core/utility/vulkan/tools.hpp"

constexpr uint32_t g_api_version{ VK_API_VERSION_1_3 };

constexpr std::vector<std::string> g_required_layers{};
constexpr std::vector<std::string> g_optional_layers{
#ifdef core_VULKAN_DEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
};

constexpr std::vector<std::string> g_required_instance_extensions{};
constexpr std::vector<std::string> g_optional_instance_extensions{
#ifdef core_VULKAN_DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
};

PFN_vkCreateDebugUtilsMessengerEXT  g_create_debug_utils_messenger_ext;
PFN_vkDestroyDebugUtilsMessengerEXT g_destroy_debug_utils_messenger_ext;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance                                instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks*              pAllocator,
    VkDebugUtilsMessengerEXT*                 pMessenger
)
{
    return g_create_debug_utils_messenger_ext(
        instance, pCreateInfo, pAllocator, pMessenger
    );
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance                   instance,
    VkDebugUtilsMessengerEXT     messenger,
    const VkAllocationCallbacks* pAllocator
)
{
    return g_destroy_debug_utils_messenger_ext(instance, messenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageFunc(
    const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    const VkDebugUtilsMessageTypeFlagsEXT        messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*  pCallbackData,
    void* /*pUserData*/
)
{
    std::ostringstream message;

    message
        << "Vulkan message "
        << vk::to_string(static_cast<const vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes
           ))
        << ":\n";
    message << std::string("\t") << "messageIDName   = <" << pCallbackData->pMessageIdName
            << ">\n";
    message << std::string("\t") << "messageIdNumber = " << pCallbackData->messageIdNumber
            << "\n";
    message << std::string("\t") << "message         = <" << pCallbackData->pMessage
            << ">\n";
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
            message
                << std::string("\t\t\t") << "objectType   = "
                << vk::to_string(
                       static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)
                   )
                << "\n";
            message << std::string("\t\t\t")
                    << "objectHandle = " << pCallbackData->pObjects[i].objectHandle
                    << "\n";
            if (pCallbackData->pObjects[i].pObjectName) {
                message << std::string("\t\t\t") << "objectName   = <"
                        << pCallbackData->pObjects[i].pObjectName << ">\n";
            }
        }
    }

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            spdlog::trace(message.str());
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            spdlog::info(message.str());
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            spdlog::warn(message.str());
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            spdlog::error(message.str());
            break;
        default: break;
    }

    return false;
}

namespace plugins::renderer {

auto application_info() noexcept -> const vk::ApplicationInfo&
{
    constexpr static vk::ApplicationInfo application_create_info{ .apiVersion =
                                                                      g_api_version };
    return application_create_info;
}

auto layers() noexcept -> std::span<const std::string>
{
    static const std::vector<std::string> layers{
        filter(core::vulkan::available_layers(), g_required_layers, g_optional_layers)
    };

    return layers;
}

auto instance_extensions() noexcept -> std::span<const std::string>
{
    static const std::vector instance_extensions{ filter(
        core::vulkan::available_instance_extensions(),
        g_required_instance_extensions,
        g_optional_instance_extensions
    ) };

    return instance_extensions;
}

auto create_debug_messenger(const vk::Instance t_instance)
    -> vk::UniqueDebugUtilsMessengerEXT
{
    auto available_extensions{ core::vulkan::available_instance_extensions() };
    if (const auto extension_supported{
            std::ranges::find(available_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
            != std::cend(available_extensions) };
        !extension_supported)
    {
        throw vk::ExtensionNotPresentError{ std::string{
                                                VK_EXT_DEBUG_UTILS_EXTENSION_NAME }
                                            + "Vulkan extension is not supported" };
    }

    g_create_debug_utils_messenger_ext =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            t_instance.getProcAddr("vkCreateDebugUtilsMessengerEXT")
        );

    g_destroy_debug_utils_messenger_ext =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            t_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT")
        );

    const vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    const vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );

    const vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info_ext{
        .messageSeverity = severity_flags,
        .messageType     = message_type_flags,
        .pfnUserCallback = &debugMessageFunc
    };

    return t_instance.createDebugUtilsMessengerEXTUnique(
        debug_utils_messenger_create_info_ext
    );
}

auto filter(
    const std::span<const std::string> t_available,
    const std::span<const std::string> t_required,
    const std::span<const std::string> t_optional
) noexcept -> std::vector<std::string>
{
    std::vector filtered{ std::from_range, t_required };

    filtered.append_range(t_optional | std::views::filter([&](const auto& optional) {
                              return std::ranges::find(t_available, optional)
                                  != std::cend(t_available);
                          }));

    return filtered;
}

auto is_adequate(
    const vk::PhysicalDevice           t_physical_device,
    const vk::SurfaceKHR               t_surface,
    const std::span<const std::string> t_required_extension_names
) noexcept -> bool
{
    return core::vulkan::supports_surface(t_physical_device, t_surface)
        && core::vulkan::supports_extensions(
               t_physical_device, t_required_extension_names
        );
}

auto choose_physical_device(
    const vk::Instance                 t_instance,
    const vk::SurfaceKHR               t_surface,
    const std::span<const std::string> t_required_extension_names
) -> vk::PhysicalDevice
{
    const auto physical_devices{ t_instance.enumeratePhysicalDevices() };

    auto adequate_devices{ physical_devices
                           | std::views::filter(
                               [t_surface, t_required_extension_names](
                                   const vk::PhysicalDevice t_physical_device
                               ) -> bool {
                                   return is_adequate(
                                       t_physical_device,
                                       t_surface,
                                       t_required_extension_names
                                   );
                               }
                           ) };

    if (std::ranges::empty(adequate_devices)) {
        return vk::PhysicalDevice{};
    }

    auto ranked_devices{ adequate_devices
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
                         | std::ranges::to<std::vector>() };

    std::ranges::sort(
        ranked_devices,
        std::ranges::greater{},
        &std::pair<vk::PhysicalDevice, unsigned>::second
    );

    return ranked_devices.front().first;
}

}   // namespace plugins::renderer
