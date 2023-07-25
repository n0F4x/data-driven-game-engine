#pragma once

#include <span>

namespace engine::vulkan {

[[nodiscard]] constexpr auto validation_layers() -> std::span<const char* const>
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

}   // namespace engine::vulkan
