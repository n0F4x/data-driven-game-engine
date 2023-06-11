#pragma once

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include "engine/core/concepts.hpp"

namespace engine::utils {

[[nodiscard]] constexpr auto
create_app_info(std::string_view t_app_name, std::string_view t_engine_name)
    -> vk::ApplicationInfo
{
    return vk::ApplicationInfo{ .pApplicationName   = t_app_name.data(),
                                .applicationVersion = VK_API_VERSION_1_0,
                                .pEngineName        = t_engine_name.data(),
                                .engineVersion      = VK_API_VERSION_1_0,
                                .apiVersion         = VK_API_VERSION_1_0 };
}

[[nodiscard]] constexpr auto create_validation_layers()
    -> std::vector<const char*>
{
#ifdef ENGINE_VULKAN_DEBUG
    if consteval {
        return { "VK_LAYER_KHRONOS_validation" };
    }
    static const std::vector<const char*> validation_layers{ "VK_LAYER_"
                                                             "KHRONOS_"
                                                             "validation" };
    return validation_layers;
#else
    return {};
#endif
}

void check_validation_layer_support(
    const std::vector<const char*>& t_validation_layers);

void check_extension_support(const std::vector<const char*>& t_extensions);

[[nodiscard]] auto
create_instance(const vk::ApplicationInfo&              t_appInfo,
                const RangeOfConcept<const char*> auto& t_validationLayers,
                const RangeOfConcept<const char*> auto& t_extensions)
    -> vk::raii::Instance
{
    check_validation_layer_support(t_validationLayers);
    check_extension_support(t_extensions);
    return {
        {},
        { .pApplicationInfo  = &t_appInfo,
         .enabledLayerCount = static_cast<uint32_t>(t_validationLayers.size()),
         .ppEnabledLayerNames     = t_validationLayers.data(),
         .enabledExtensionCount   = static_cast<uint32_t>(t_extensions.size()),
         .ppEnabledExtensionNames = t_extensions.data() }
    };
}

}   // namespace engine::utils
