module;

#include <concepts>

export module ddge.modules.vulkan.structure_chains.comprehensive_feature_struct_c;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.vulkan1x_feature_struct_c;

namespace ddge::vulkan {

export template <typename T>
concept comprehensive_feature_struct_c = std::same_as<T, vk::PhysicalDeviceFeatures>
                                      || vulkan1x_feature_struct_c<T>;

}   // namespace ddge::vulkan
