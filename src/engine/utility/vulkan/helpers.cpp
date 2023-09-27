#include "helpers.hpp"

#include <iostream>

#include <vulkan/vulkan_to_string.hpp>

#include "SFML/Window/Vulkan.hpp"

namespace {

auto create_instance_extensions() noexcept
{
    std::vector extensions{ sf::Vulkan::getGraphicsRequiredInstanceExtensions(
    ) };
#ifdef ENGINE_VULKAN_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    return extensions;
}

}   // namespace

namespace engine::utils::vulkan {

auto instance_extensions() noexcept -> std::span<const char* const>
{
    static const std::vector<const char*> instance_extensions{
        create_instance_extensions()
    };
    return instance_extensions;
}

}   // namespace engine::utils::vulkan

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

namespace engine::utils::vulkan {

auto create_debug_messenger(vk::Instance t_instance) noexcept
    -> std::optional<vk::DebugUtilsMessengerEXT>
{
    auto props = vk::enumerateInstanceExtensionProperties();
    if (props.result != vk::Result::eSuccess) {
        return std::nullopt;
    }

    auto propertyIterator = std::find_if(
        props.value.begin(),
        props.value.end(),
        [](const vk::ExtensionProperties& ep) {
            return strcmp(ep.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
                == 0;
        }
    );
    if (propertyIterator == props.value.end()) {
        return std::nullopt;
    }

    pfnVkCreateDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            t_instance.getProcAddr("vkCreateDebugUtilsMessengerEXT")
        );
    if (pfnVkCreateDebugUtilsMessengerEXT == nullptr) {
        return std::nullopt;
    }

    pfnVkDestroyDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            t_instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT")
        );
    if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr) {
        return std::nullopt;
    }

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );
    auto debugUtilsMessenger = t_instance.createDebugUtilsMessengerEXT(
        vk::DebugUtilsMessengerCreateInfoEXT{ .messageSeverity = severityFlags,
                                              .messageType = messageTypeFlags,
                                              .pfnUserCallback =
                                                  &debugMessageFunc }
    );
    if (debugUtilsMessenger.result == vk::Result::eSuccess) {
        return debugUtilsMessenger.value;
    }

    return std::nullopt;
}

}   // namespace engine::utils::vulkan
