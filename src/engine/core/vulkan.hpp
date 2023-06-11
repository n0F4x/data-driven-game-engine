#pragma once

#include <algorithm>
#include <ranges>
#include <span>
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
    -> std::span<const char* const>
{
#ifdef ENGINE_VULKAN_DEBUG
    if consteval {
        return { { "VK_LAYER_KHRONOS_validation" } };
    }
    static const std::array validation_layers{ "VK_LAYER_KHRONOS_validation" };
    return validation_layers;
#else
    return {};
#endif
}

void check_validation_layer_support(
    std::span<const char* const> t_validation_layers);

void check_extension_support(std::span<const char* const> t_extensions);

[[nodiscard]] auto
create_instance(const vk::ApplicationInfo&   t_appInfo,
                std::span<const char* const> t_validationLayers,
                std::span<const char* const> t_extensions)
    -> vk::raii::Instance;

}   // namespace engine::utils
