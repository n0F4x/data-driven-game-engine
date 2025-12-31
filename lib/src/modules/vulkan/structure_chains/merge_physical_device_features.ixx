export module ddge.modules.vulkan.structure_chains.merge_physical_device_features;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.comprehensive_feature_struct_c;
import ddge.modules.vulkan.structure_chains.part_of_comprehensive_feature_struct_c;
import ddge.modules.vulkan.structure_chains.feature_struct_c;

namespace ddge::vulkan {

export template <feature_struct_c FeatureStruct_T>
constexpr auto
    merge_physical_device_features(FeatureStruct_T& inout, const FeatureStruct_T& in)
        -> void;

export template <
    comprehensive_feature_struct_c ComprehensiveFeatureStruct_T,
    part_of_comprehensive_feature_struct_c<ComprehensiveFeatureStruct_T>
        IndividualFeatureStruct_T>
constexpr auto merge_physical_device_features(
    ComprehensiveFeatureStruct_T&    inout,
    const IndividualFeatureStruct_T& in
) -> void;

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

// TODO: use reflection

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan11Features&           inout,
    const vk::PhysicalDevice16BitStorageFeatures& in
) -> void
{
    inout.storageBuffer16BitAccess |= in.storageBuffer16BitAccess;
    inout.uniformAndStorageBuffer16BitAccess |= in.uniformAndStorageBuffer16BitAccess;
    inout.storagePushConstant16 |= in.storagePushConstant16;
    inout.storageInputOutput16 |= in.storageInputOutput16;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan11Features&        inout,
    const vk::PhysicalDeviceMultiviewFeatures& in
) -> void
{
    inout.multiview |= in.multiview;
    inout.multiviewGeometryShader |= in.multiviewGeometryShader;
    inout.multiviewTessellationShader |= in.multiviewTessellationShader;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan11Features&               inout,
    const vk::PhysicalDeviceVariablePointersFeatures& in
) -> void
{
    inout.variablePointersStorageBuffer |= in.variablePointersStorageBuffer;
    inout.variablePointers |= in.variablePointers;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan11Features&              inout,
    const vk::PhysicalDeviceProtectedMemoryFeatures& in
) -> void
{
    inout.protectedMemory |= in.protectedMemory;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan11Features&                     inout,
    const vk::PhysicalDeviceSamplerYcbcrConversionFeatures& in
) -> void
{
    inout.samplerYcbcrConversion |= in.samplerYcbcrConversion;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan11Features&                   inout,
    const vk::PhysicalDeviceShaderDrawParametersFeatures& in
) -> void
{
    inout.shaderDrawParameters |= in.shaderDrawParameters;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&          inout,
    const vk::PhysicalDevice8BitStorageFeatures& in
) -> void
{
    inout.storageBuffer8BitAccess |= in.storageBuffer8BitAccess;
    inout.uniformAndStorageBuffer8BitAccess |= in.uniformAndStorageBuffer8BitAccess;
    inout.storagePushConstant8 |= in.storagePushConstant8;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                inout,
    const vk::PhysicalDeviceShaderAtomicInt64Features& in
) -> void
{
    inout.shaderBufferInt64Atomics |= in.shaderBufferInt64Atomics;
    inout.shaderSharedInt64Atomics |= in.shaderSharedInt64Atomics;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                inout,
    const vk::PhysicalDeviceShaderFloat16Int8Features& in
) -> void
{
    inout.shaderFloat16 |= in.shaderFloat16;
    inout.shaderInt8 |= in.shaderInt8;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                 inout,
    const vk::PhysicalDeviceDescriptorIndexingFeatures& in
) -> void
{
    inout.shaderInputAttachmentArrayDynamicIndexing |=
        in.shaderInputAttachmentArrayDynamicIndexing;
    inout.shaderUniformTexelBufferArrayDynamicIndexing |=
        in.shaderUniformTexelBufferArrayDynamicIndexing;
    inout.shaderStorageTexelBufferArrayDynamicIndexing |=
        in.shaderStorageTexelBufferArrayDynamicIndexing;
    inout.shaderUniformBufferArrayNonUniformIndexing |=
        in.shaderUniformBufferArrayNonUniformIndexing;
    inout.shaderSampledImageArrayNonUniformIndexing |=
        in.shaderSampledImageArrayNonUniformIndexing;
    inout.shaderStorageBufferArrayNonUniformIndexing |=
        in.shaderStorageBufferArrayNonUniformIndexing;
    inout.shaderStorageImageArrayNonUniformIndexing |=
        in.shaderStorageImageArrayNonUniformIndexing;
    inout.shaderInputAttachmentArrayNonUniformIndexing |=
        in.shaderInputAttachmentArrayNonUniformIndexing;
    inout.shaderUniformTexelBufferArrayNonUniformIndexing |=
        in.shaderUniformTexelBufferArrayNonUniformIndexing;
    inout.shaderStorageTexelBufferArrayNonUniformIndexing |=
        in.shaderStorageTexelBufferArrayNonUniformIndexing;
    inout.descriptorBindingUniformBufferUpdateAfterBind |=
        in.descriptorBindingUniformBufferUpdateAfterBind;
    inout.descriptorBindingSampledImageUpdateAfterBind |=
        in.descriptorBindingSampledImageUpdateAfterBind;
    inout.descriptorBindingStorageImageUpdateAfterBind |=
        in.descriptorBindingStorageImageUpdateAfterBind;
    inout.descriptorBindingStorageBufferUpdateAfterBind |=
        in.descriptorBindingStorageBufferUpdateAfterBind;
    inout.descriptorBindingUniformTexelBufferUpdateAfterBind |=
        in.descriptorBindingUniformTexelBufferUpdateAfterBind;
    inout.descriptorBindingStorageTexelBufferUpdateAfterBind |=
        in.descriptorBindingStorageTexelBufferUpdateAfterBind;
    inout.descriptorBindingUpdateUnusedWhilePending |=
        in.descriptorBindingUpdateUnusedWhilePending;
    inout.descriptorBindingPartiallyBound |= in.descriptorBindingPartiallyBound;
    inout.descriptorBindingVariableDescriptorCount |=
        in.descriptorBindingVariableDescriptorCount;
    inout.runtimeDescriptorArray |= in.runtimeDescriptorArray;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                inout,
    const vk::PhysicalDeviceScalarBlockLayoutFeatures& in
) -> void
{
    inout.scalarBlockLayout |= in.scalarBlockLayout;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                   inout,
    const vk::PhysicalDeviceImagelessFramebufferFeatures& in
) -> void
{
    inout.imagelessFramebuffer |= in.imagelessFramebuffer;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                          inout,
    const vk::PhysicalDeviceUniformBufferStandardLayoutFeatures& in
) -> void
{
    inout.uniformBufferStandardLayout |= in.uniformBufferStandardLayout;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                          inout,
    const vk::PhysicalDeviceShaderSubgroupExtendedTypesFeatures& in
) -> void
{
    inout.shaderSubgroupExtendedTypes |= in.shaderSubgroupExtendedTypes;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                          inout,
    const vk::PhysicalDeviceSeparateDepthStencilLayoutsFeatures& in
) -> void
{
    inout.separateDepthStencilLayouts |= in.separateDepthStencilLayouts;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&             inout,
    const vk::PhysicalDeviceHostQueryResetFeatures& in
) -> void
{
    inout.hostQueryReset |= in.hostQueryReset;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                inout,
    const vk::PhysicalDeviceTimelineSemaphoreFeatures& in
) -> void
{
    inout.timelineSemaphore |= in.timelineSemaphore;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                  inout,
    const vk::PhysicalDeviceBufferDeviceAddressFeatures& in
) -> void
{
    inout.bufferDeviceAddress |= in.bufferDeviceAddress;
    inout.bufferDeviceAddressCaptureReplay |= in.bufferDeviceAddressCaptureReplay;
    inout.bufferDeviceAddressMultiDevice |= in.bufferDeviceAddressMultiDevice;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan12Features&                inout,
    const vk::PhysicalDeviceVulkanMemoryModelFeatures& in
) -> void
{
    inout.vulkanMemoryModel |= in.vulkanMemoryModel;
    inout.vulkanMemoryModelDeviceScope |= in.vulkanMemoryModelDeviceScope;
    inout.vulkanMemoryModelAvailabilityVisibilityChains |=
        in.vulkanMemoryModelAvailabilityVisibilityChains;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&              inout,
    const vk::PhysicalDeviceImageRobustnessFeatures& in
) -> void
{
    inout.robustImageAccess |= in.robustImageAccess;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                 inout,
    const vk::PhysicalDeviceInlineUniformBlockFeatures& in
) -> void
{
    inout.inlineUniformBlock |= in.inlineUniformBlock;
    inout.descriptorBindingInlineUniformBlockUpdateAfterBind |=
        in.descriptorBindingInlineUniformBlockUpdateAfterBind;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                           inout,
    const vk::PhysicalDevicePipelineCreationCacheControlFeatures& in
) -> void
{
    inout.pipelineCreationCacheControl |= in.pipelineCreationCacheControl;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&          inout,
    const vk::PhysicalDevicePrivateDataFeatures& in
) -> void
{
    inout.privateData |= in.privateData;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                             inout,
    const vk::PhysicalDeviceShaderDemoteToHelperInvocationFeatures& in
) -> void
{
    inout.shaderDemoteToHelperInvocation |= in.shaderDemoteToHelperInvocation;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                        inout,
    const vk::PhysicalDeviceShaderTerminateInvocationFeatures& in
) -> void
{
    inout.shaderTerminateInvocation |= in.shaderTerminateInvocation;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                  inout,
    const vk::PhysicalDeviceSubgroupSizeControlFeatures& in
) -> void
{
    inout.subgroupSizeControl |= in.subgroupSizeControl;
    inout.computeFullSubgroups |= in.computeFullSubgroups;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&               inout,
    const vk::PhysicalDeviceSynchronization2Features& in
) -> void
{
    inout.synchronization2 |= in.synchronization2;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                        inout,
    const vk::PhysicalDeviceTextureCompressionASTCHDRFeatures& in
) -> void
{
    inout.textureCompressionASTC_HDR |= in.textureCompressionASTC_HDR;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                            inout,
    const vk::PhysicalDeviceZeroInitializeWorkgroupMemoryFeatures& in
) -> void
{
    inout.shaderZeroInitializeWorkgroupMemory |= in.shaderZeroInitializeWorkgroupMemory;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&               inout,
    const vk::PhysicalDeviceDynamicRenderingFeatures& in
) -> void
{
    inout.dynamicRendering |= in.dynamicRendering;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&                      inout,
    const vk::PhysicalDeviceShaderIntegerDotProductFeatures& in
) -> void
{
    inout.shaderIntegerDotProduct |= in.shaderIntegerDotProduct;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan13Features&           inout,
    const vk::PhysicalDeviceMaintenance4Features& in
) -> void
{
    inout.maintenance4 |= in.maintenance4;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                  inout,
    const vk::PhysicalDeviceGlobalPriorityQueryFeatures& in
) -> void
{
    inout.globalPriorityQuery |= in.globalPriorityQuery;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                   inout,
    const vk::PhysicalDeviceShaderSubgroupRotateFeatures& in
) -> void
{
    inout.shaderSubgroupRotate |= in.shaderSubgroupRotate;
    inout.shaderSubgroupRotateClustered |= in.shaderSubgroupRotateClustered;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                   inout,
    const vk::PhysicalDeviceShaderFloatControls2Features& in
) -> void
{
    inout.shaderFloatControls2 |= in.shaderFloatControls2;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                 inout,
    const vk::PhysicalDeviceShaderExpectAssumeFeatures& in
) -> void
{
    inout.shaderExpectAssume |= in.shaderExpectAssume;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                inout,
    const vk::PhysicalDeviceLineRasterizationFeatures& in
) -> void
{
    inout.rectangularLines |= in.rectangularLines;
    inout.bresenhamLines |= in.bresenhamLines;
    inout.smoothLines |= in.smoothLines;
    inout.stippledRectangularLines |= in.stippledRectangularLines;
    inout.stippledBresenhamLines |= in.stippledBresenhamLines;
    inout.stippledSmoothLines |= in.stippledSmoothLines;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                     inout,
    const vk::PhysicalDeviceVertexAttributeDivisorFeatures& in
) -> void
{
    inout.vertexAttributeInstanceRateDivisor |= in.vertexAttributeInstanceRateDivisor;
    inout.vertexAttributeInstanceRateZeroDivisor |=
        in.vertexAttributeInstanceRateZeroDivisor;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&             inout,
    const vk::PhysicalDeviceIndexTypeUint8Features& in
) -> void
{
    inout.indexTypeUint8 |= in.indexTypeUint8;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                        inout,
    const vk::PhysicalDeviceDynamicRenderingLocalReadFeatures& in
) -> void
{
    inout.dynamicRenderingLocalRead |= in.dynamicRenderingLocalRead;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&           inout,
    const vk::PhysicalDeviceMaintenance5Features& in
) -> void
{
    inout.maintenance5 |= in.maintenance5;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&           inout,
    const vk::PhysicalDeviceMaintenance6Features& in
) -> void
{
    inout.maintenance6 |= in.maintenance6;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                      inout,
    const vk::PhysicalDevicePipelineProtectedAccessFeatures& in
) -> void
{
    inout.pipelineProtectedAccess |= in.pipelineProtectedAccess;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&                 inout,
    const vk::PhysicalDevicePipelineRobustnessFeatures& in
) -> void
{
    inout.pipelineRobustness |= in.pipelineRobustness;
}

constexpr auto merge_physical_device_features_impl(
    vk::PhysicalDeviceVulkan14Features&            inout,
    const vk::PhysicalDeviceHostImageCopyFeatures& in
) -> void
{
    inout.hostImageCopy |= in.hostImageCopy;
}

template <
    comprehensive_feature_struct_c ComprehensiveFeatureStruct_T,
    part_of_comprehensive_feature_struct_c<ComprehensiveFeatureStruct_T>
        IndividualFeatureStruct_T>
constexpr auto merge_physical_device_features(
    ComprehensiveFeatureStruct_T&    inout,
    const IndividualFeatureStruct_T& in
) -> void
{
    merge_physical_device_features_impl(inout, in);
}

}   // namespace ddge::vulkan
