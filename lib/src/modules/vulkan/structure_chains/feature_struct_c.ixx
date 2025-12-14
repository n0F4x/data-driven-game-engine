module;

#include <concepts>

export module ddge.modules.vulkan.structure_chains.feature_struct_c;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.extends_struct_c;
import ddge.modules.vulkan.structure_chains.promoted_feature_struct_c;

namespace ddge::vulkan {

export template <typename T>
concept feature_struct_c = extends_struct_c<T, vk::PhysicalDeviceFeatures2>
                        || std::same_as<T, vk::PhysicalDeviceFeatures>
                        || promoted_feature_struct_c<T>;

}   // namespace ddge::vulkan
