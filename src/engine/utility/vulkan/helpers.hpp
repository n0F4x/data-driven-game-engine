#pragma once

#include <optional>
#include <span>

#include <vulkan/vulkan.hpp>

namespace engine::utils::vulkan {

[[nodiscard]] constexpr auto api_version() noexcept -> uint32_t
{
    return VK_API_VERSION_1_3;
}

[[nodiscard]] constexpr auto validation_layers() noexcept
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

[[nodiscard]] auto instance_extensions() noexcept
    -> std::span<const char* const>;

[[nodiscard]] auto create_debug_messenger(vk::Instance t_instance) noexcept
    -> std::optional<vk::DebugUtilsMessengerEXT>;

}   // namespace engine::utils::vulkan
