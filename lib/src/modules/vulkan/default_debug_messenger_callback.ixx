module;

#include "vulkan/vk_platform.h"

export module ddge.modules.vulkan.default_debug_messenger_callback;

import vulkan_hpp;

namespace ddge::vulkan {

export VKAPI_ATTR auto VKAPI_CALL default_debug_messenger_callback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
    vk::DebugUtilsMessageTypeFlagsEXT             type,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                         pUserData
) -> vk::Bool32;

}   // namespace ddge::vulkan
