module;

#include <type_traits>

export module ddge.modules.vulkan.result.TypedResultCode;

import vulkan_hpp;

namespace ddge::vulkan {

export template <vk::Result value_T>
struct TypedResultCode {
    constexpr static std::integral_constant<vk::Result, value_T> value;
};

}   // namespace ddge::vulkan
