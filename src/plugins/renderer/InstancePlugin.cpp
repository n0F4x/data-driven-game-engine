#include "InstancePlugin.hpp"

#include <ranges>

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include "app/App.hpp"
#include "core/config/vulkan.hpp"
#include "core/renderer/base/instance/Instance.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_message(
    const VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    const VkDebugUtilsMessageTypeFlagsEXT        message_types,
    const VkDebugUtilsMessengerCallbackDataEXT*  callback_data,
    void* /*pUserData*/
)
{
    std::ostringstream message;

    message << "Vulkan message "
            << vk::to_string(
                   static_cast<const vk::DebugUtilsMessageTypeFlagsEXT>(message_types)
               )
            << ":\n";
    message << std::string("\t") << "messageIDName   = <" << callback_data->pMessageIdName
            << ">\n";
    message << std::string("\t") << "messageIdNumber = " << callback_data->messageIdNumber
            << "\n";
    message << std::string("\t") << "message         = <" << callback_data->pMessage
            << ">\n";
    if (0 < callback_data->queueLabelCount) {
        message << std::string("\t") << "Queue Labels:\n";
        for (uint32_t i = 0; i < callback_data->queueLabelCount; i++) {
            message << std::string("\t\t") << "labelName = <"
                    << callback_data->pQueueLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < callback_data->cmdBufLabelCount) {
        message << std::string("\t") << "CommandBuffer Labels:\n";
        for (uint32_t i = 0; i < callback_data->cmdBufLabelCount; i++) {
            message << std::string("\t\t") << "labelName = <"
                    << callback_data->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < callback_data->objectCount) {
        message << std::string("\t") << "Objects:\n";
        for (uint32_t i = 0; i < callback_data->objectCount; i++) {
            message << std::string("\t\t") << "Object " << i << "\n";
            message
                << std::string("\t\t\t") << "objectType   = "
                << vk::to_string(
                       static_cast<vk::ObjectType>(callback_data->pObjects[i].objectType)
                   )
                << "\n";
            message << std::string("\t\t\t")
                    << "objectcache::Handle = " << callback_data->pObjects[i].objectHandle
                    << "\n";
            if (callback_data->pObjects[i].pObjectName) {
                message << std::string("\t\t\t") << "objectName   = <"
                        << callback_data->pObjects[i].pObjectName << ">\n";
            }
        }
    }

    switch (static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(message_severity)) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            spdlog::trace(message.str());
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            spdlog::info(message.str());
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            spdlog::warn(message.str());
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            spdlog::error(message.str());
            break;
        default: break;
    }

    return vk::False;
}

[[maybe_unused]]
static auto set_debug_messenger(vkb::InstanceBuilder& builder) -> void
{
    builder.set_debug_callback(debug_message);

    const vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    builder.set_debug_messenger_severity(
        severity_flags.operator vk::DebugUtilsMessageSeverityFlagsEXT::MaskType()
    );

    const vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );
    builder.set_debug_messenger_type(
        message_type_flags.operator vk::DebugUtilsMessageTypeFlagsEXT::MaskType()
    );
}

static auto required_instance_settings_are_available(
    [[maybe_unused]] const vkb::SystemInfo& system_info
) -> bool
{
#ifdef ENGINE_VULKAN_DEBUG
    return system_info.validation_layers_available && system_info.debug_utils_available;
#else
    return true;
#endif
}

static auto enable_instance_settings(
    const vkb::SystemInfo&,
    [[maybe_unused]] vkb::InstanceBuilder& instance_builder
) -> void
{
#ifdef ENGINE_VULKAN_DEBUG
    instance_builder.enable_validation_layers();
    set_debug_messenger(instance_builder);
#endif
}

namespace plugins::renderer {

InstancePlugin::InstancePlugin()
{
    emplace_dependency(Dependency{
        .required_instance_settings_are_available =
            ::required_instance_settings_are_available,
        .enable_instance_settings = ::enable_instance_settings });
}

auto InstancePlugin::operator()(App& app) const -> void
{
    core::config::vulkan::init();

    const auto system_info_result{ vkb::SystemInfo::get_system_info() };
    if (!system_info_result.has_value()) {
        SPDLOG_ERROR(system_info_result.error().message());
        return;
    }
    const vkb::SystemInfo& system_info{ system_info_result.value() };

    // TODO: use std::bind_back
    if (!std::ranges::all_of(
            m_dependencies | std::views::filter([](const Dependency& dependency) {
                return dependency.required_instance_settings_are_available != nullptr;
            }),
            [&system_info](const Dependency& dependency) {
                return dependency.required_instance_settings_are_available(system_info);
            }
        ))
    {
        return;
    }

    vkb::InstanceBuilder instance_builder{
        core::config::vulkan::dispatcher().vkGetInstanceProcAddr
    };

    // TODO: use std::bind_back
    for (const Dependency& dependency : m_dependencies) {
        if (dependency.enable_instance_settings) {
            dependency.enable_instance_settings(system_info, instance_builder);
        }
    }

    const vkb::Result<vkb::Instance> instance_result{ instance_builder.build() };
    if (!instance_result.has_value()) {
        SPDLOG_ERROR(instance_result.error().message());
        return;
    }

    const auto& instance{
        app.resources.emplace<core::renderer::base::Instance>(instance_result.value())
    };

    core::config::vulkan::init(instance.get());
}

}   // namespace plugins::renderer
