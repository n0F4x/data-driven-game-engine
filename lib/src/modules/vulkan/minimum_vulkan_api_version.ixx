module;

#include <cstdint>

export module ddge.modules.vulkan.minimum_vulkan_api_version;

import vulkan_hpp;

namespace ddge::vulkan {

export [[nodiscard]]
constexpr auto minimum_vulkan_api_version() noexcept -> uint32_t;

}   // namespace ddge::vulkan

module :private;

constexpr auto ddge::vulkan::minimum_vulkan_api_version() noexcept -> uint32_t
{
    constexpr static uint32_t result{ vk::ApiVersion14 };

    return result;
}
