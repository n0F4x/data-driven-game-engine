export module ddge.modules.vulkan.structure_chains.match_physical_device_features;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.feature_struct_c;

namespace ddge::vulkan {

export template <feature_struct_c FeatureStruct_T>
constexpr auto match_physical_device_features(
    const FeatureStruct_T& base,
    const FeatureStruct_T& target
) -> bool;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

constexpr auto
    match_feature_struct_member(const vk::StructureType, const vk::StructureType) -> bool
{
    return true;
}

constexpr auto match_feature_struct_member(void* const, void* const) -> bool
{
    return true;
}

constexpr auto match_feature_struct_member(const vk::Bool32& base, const vk::Bool32 target)
    -> bool
{
    return !base || base == target;
}

template <feature_struct_c FeatureStruct_T>
constexpr auto match_physical_device_features(
    const FeatureStruct_T& base,
    const FeatureStruct_T& target
) -> bool
{
    auto& [... base_members]{ base };
    const auto& [... target_members]{ target };
    return (match_feature_struct_member(base_members, target_members) && ...);
}

}   // namespace ddge::vulkan
