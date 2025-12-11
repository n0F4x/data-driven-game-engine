module;

#include <format>
#include <ranges>
#include <sstream>

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

module ddge.modules.vulkan.default_debug_messenger_callback;

import ddge.modules.config.engine_name;

namespace ddge::vulkan {

auto logger{ [] {
    auto result{
        spdlog::stdout_color_mt(std::format("{} Vulkan", config::engine_name()))
    };
    result->set_level(spdlog::level::level_enum::debug);
    return result;
}() };

auto default_debug_messenger_callback(
    const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    const vk::DebugUtilsMessageTypeFlagsEXT        type,
    const vk::DebugUtilsMessengerCallbackDataEXT*  pCallbackData,
    void* const
) -> vk::Bool32
{
    std::ostringstream message;

    message << std::format(
        "Vulkan message [] - '{} ({}):'\n" "- {}\n",
        vk::to_string(type),
        pCallbackData->pMessageIdName,
        pCallbackData->messageIdNumber,
        pCallbackData->pMessage
    );

    if (pCallbackData->queueLabelCount > 0) {
        message << "- Queue labels:\n";
        for (const char* label_name :
             std::span{ pCallbackData->pQueueLabels, pCallbackData->queueLabelCount }
                 | std::views::transform(&vk::DebugUtilsLabelEXT::pLabelName))
        {
            message << std::format("\t- \"{}\"\n", label_name);
        }
    }

    if (pCallbackData->cmdBufLabelCount > 0) {
        message << "- Command buffer labels:\n";
        for (const char* label_name :
             std::span{ pCallbackData->pCmdBufLabels, pCallbackData->cmdBufLabelCount }
                 | std::views::transform(&vk::DebugUtilsLabelEXT::pLabelName))
        {
            message << std::format("\t- \"{}\"\n", label_name);
        }
    }

    if (pCallbackData->objectCount > 0) {
        message << "- Objects:\n";
        for (const vk::DebugUtilsObjectNameInfoEXT& object_info :
             std::span{ pCallbackData->pObjects, pCallbackData->objectCount })
        {
            const char* const object_name{
                object_info.pObjectName ? object_info.pObjectName : ""   //
            };
            message << std::format(
                "\t- {} ({}) {}\n",
                vk::to_string(object_info.objectType),
                object_info.objectHandle,
                object_name
            );
        }
    }

    switch (severity) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            logger->debug(message.str());
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            logger->info(message.str());
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            logger->warn(message.str());
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            logger->error(message.str());
            break;
        default: break;
    }

    return vk::False;
}

}   // namespace ddge::vulkan
