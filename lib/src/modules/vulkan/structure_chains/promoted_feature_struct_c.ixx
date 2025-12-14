module;

#include <concepts>

export module ddge.modules.vulkan.structure_chains.promoted_feature_struct_c;

import vulkan_hpp;

namespace ddge::vulkan {

export template <typename T>
concept promoted_feature_struct_c = std::same_as<T, vk::PhysicalDeviceVulkan11Features>
                                 || std::same_as<T, vk::PhysicalDeviceVulkan12Features>
                                 || std::same_as<T, vk::PhysicalDeviceVulkan13Features>
                                 || std::same_as<T, vk::PhysicalDeviceVulkan14Features>;

}   // namespace ddge::vulkan
