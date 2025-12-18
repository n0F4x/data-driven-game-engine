export module ddge.modules.vulkan.structure_chains.merge_physical_device_features;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.feature_struct_c;

namespace ddge::vulkan {

export template <feature_struct_c FeatureStruct_T>
constexpr auto
    merge_physical_device_features(FeatureStruct_T& inout, const FeatureStruct_T& in)
        -> void;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

constexpr auto
    merge_feature_struct_member(const vk::StructureType, const vk::StructureType) -> void
{}

constexpr auto merge_feature_struct_member(void* const, void* const) -> void {}

constexpr auto merge_feature_struct_member(vk::Bool32& inout, const vk::Bool32 in) -> void
{
    inout |= in;
}

template <feature_struct_c FeatureStruct_T>
constexpr auto
    merge_physical_device_features(FeatureStruct_T& inout, const FeatureStruct_T& in)
        -> void
{
    auto& [... inout_members]{ inout };
    const auto& [... in_members]{ in };
    (merge_feature_struct_member(inout_members, in_members), ...);
}

}   // namespace ddge::vulkan
