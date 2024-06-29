#include "helpers.hpp"

#include <spdlog/spdlog.h>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>

namespace plugins::renderer {

VKAPI_ATTR VkBool32 VKAPI_CALL debug_message(
    const VkDebugUtilsMessageSeverityFlagBitsEXT t_message_severity,
    const VkDebugUtilsMessageTypeFlagsEXT        t_message_types,
    const VkDebugUtilsMessengerCallbackDataEXT*  t_callback_data,
    void* /*pUserData*/
)
{
    std::ostringstream message;

    message << "Vulkan message "
            << vk::to_string(
                   static_cast<const vk::DebugUtilsMessageTypeFlagsEXT>(t_message_types)
               )
            << ":\n";
    message << std::string("\t") << "messageIDName   = <"
            << t_callback_data->pMessageIdName << ">\n";
    message << std::string("\t")
            << "messageIdNumber = " << t_callback_data->messageIdNumber << "\n";
    message << std::string("\t") << "message         = <" << t_callback_data->pMessage
            << ">\n";
    if (0 < t_callback_data->queueLabelCount) {
        message << std::string("\t") << "Queue Labels:\n";
        for (uint32_t i = 0; i < t_callback_data->queueLabelCount; i++) {
            message << std::string("\t\t") << "labelName = <"
                    << t_callback_data->pQueueLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < t_callback_data->cmdBufLabelCount) {
        message << std::string("\t") << "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < t_callback_data->cmdBufLabelCount; i++) {
            message << std::string("\t\t") << "labelName = <"
                    << t_callback_data->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < t_callback_data->objectCount) {
        message << std::string("\t") << "Objects:\n";
        for (uint32_t i = 0; i < t_callback_data->objectCount; i++) {
            message << std::string("\t\t") << "Object " << i << "\n";
            message << std::string("\t\t\t") << "objectType   = "
                    << vk::to_string(static_cast<vk::ObjectType>(
                           t_callback_data->pObjects[i].objectType
                       ))
                    << "\n";
            message << std::string("\t\t\t") << "objectcache::Handle = "
                    << t_callback_data->pObjects[i].objectHandle << "\n";
            if (t_callback_data->pObjects[i].pObjectName) {
                message << std::string("\t\t\t") << "objectName   = <"
                        << t_callback_data->pObjects[i].pObjectName << ">\n";
            }
        }
    }

    switch (t_message_severity) {
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

    return vk::False;
}

[[maybe_unused]]
static auto set_debug_messenger(vkb::InstanceBuilder& t_builder) -> void
{
    t_builder.set_debug_callback(debug_message);

    const vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    t_builder.set_debug_messenger_severity(
        severity_flags.operator vk::DebugUtilsMessageSeverityFlagsEXT::MaskType()
    );

    const vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );
    t_builder.set_debug_messenger_type(
        message_type_flags.operator vk::DebugUtilsMessageTypeFlagsEXT::MaskType()
    );
}

auto default_required_instance_settings_are_available(
    [[maybe_unused]] const vkb::SystemInfo& t_system_info
) -> bool
{
#ifdef ENGINE_VULKAN_DEBUG
    return t_system_info.validation_layers_available
        && t_system_info.debug_utils_available;
#else
    return true;
#endif
}

auto enable_default_instance_settings(
    const vkb::SystemInfo&,
    [[maybe_unused]] vkb::InstanceBuilder& t_builder
) -> void
{
#ifdef ENGINE_VULKAN_DEBUG
    t_builder.enable_validation_layers();
    set_debug_messenger(t_builder);
#endif
}

auto log_renderer_setup(const vkb::Device& t_device) -> void
{
    try {
        const auto instance_version{ t_device.instance_version };

        SPDLOG_INFO(
            "Created Vulkan Instance with version: {}.{}.{}",
            VK_VERSION_MAJOR(instance_version),
            VK_VERSION_MINOR(instance_version),
            VK_VERSION_PATCH(instance_version)
        );

        const auto properties{
            vk::PhysicalDevice(t_device.physical_device.physical_device).getProperties()
        };

        SPDLOG_INFO(
            "Chose GPU({}) with Vulkan version: {}.{}.{}",
            t_device.physical_device.name,
            VK_VERSION_MAJOR(properties.apiVersion),
            VK_VERSION_MINOR(properties.apiVersion),
            VK_VERSION_PATCH(properties.apiVersion)
        );

        std::string enabled_extensions{ "\nEnabled device extensions:" };
        for (const auto& extension : t_device.physical_device.get_extensions()) {
            enabled_extensions += '\n';
            enabled_extensions += '\t';
            enabled_extensions += extension;
        }
        SPDLOG_DEBUG(enabled_extensions);
    } catch (const vk::Error& t_error) {
        SPDLOG_ERROR(t_error.what());
    }
}
}   // namespace plugins::renderer
