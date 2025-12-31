export module ddge.modules.vulkan.structure_chains.remove_physical_device_features;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.comprehensive_feature_struct_c;
import ddge.modules.vulkan.structure_chains.feature_struct_c;
import ddge.modules.vulkan.structure_chains.part_of_comprehensive_feature_struct_c;

namespace ddge::vulkan {

export template <feature_struct_c FeatureStruct_T>
constexpr auto
    remove_physical_device_features(FeatureStruct_T& inout, const FeatureStruct_T& in)
        -> void;

export template <
    comprehensive_feature_struct_c ComprehensiveFeatureStruct_T,
    part_of_comprehensive_feature_struct_c<ComprehensiveFeatureStruct_T>
        IndividualFeatureStruct_T>
constexpr auto remove_physical_device_features(
    IndividualFeatureStruct_T&          inout,
    const ComprehensiveFeatureStruct_T& in
) -> void;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

constexpr auto
    remove_feature_struct_member(const vk::StructureType, const vk::StructureType) -> void
{}

constexpr auto remove_feature_struct_member(void* const, void* const) -> void {}

constexpr auto remove_feature_struct_member(vk::Bool32& inout, const vk::Bool32 in)
    -> void
{
    if (in) {
        inout = vk::False;
    }
}

template <feature_struct_c FeatureStruct_T>
constexpr auto
    remove_physical_device_features(FeatureStruct_T& inout, const FeatureStruct_T& in)
        -> void
{
    auto& [... inout_members]{ inout };
    const auto& [... in_members]{ in };
    (remove_feature_struct_member(inout_members, in_members), ...);
}

// TODO: use reflection

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDevice16BitStorageFeatures&   inout,
    const vk::PhysicalDeviceVulkan11Features& in
) -> void
{
    remove_feature_struct_member(
        inout.storageBuffer16BitAccess, in.storageBuffer16BitAccess
    );
    remove_feature_struct_member(
        inout.uniformAndStorageBuffer16BitAccess, in.uniformAndStorageBuffer16BitAccess
    );
    remove_feature_struct_member(inout.storagePushConstant16, in.storagePushConstant16);
    remove_feature_struct_member(inout.storageInputOutput16, in.storageInputOutput16);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceMultiviewFeatures&      inout,
    const vk::PhysicalDeviceVulkan11Features& in
) -> void
{
    remove_feature_struct_member(inout.multiview, in.multiview);
    remove_feature_struct_member(
        inout.multiviewGeometryShader, in.multiviewGeometryShader
    );
    remove_feature_struct_member(
        inout.multiviewTessellationShader, in.multiviewTessellationShader
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceVariablePointersFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&   in
) -> void
{
    remove_feature_struct_member(
        inout.variablePointersStorageBuffer, in.variablePointersStorageBuffer
    );
    remove_feature_struct_member(inout.variablePointers, in.variablePointers);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceProtectedMemoryFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&  in
) -> void
{
    remove_feature_struct_member(inout.protectedMemory, in.protectedMemory);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceSamplerYcbcrConversionFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&         in
) -> void
{
    remove_feature_struct_member(inout.samplerYcbcrConversion, in.samplerYcbcrConversion);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderDrawParametersFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&       in
) -> void
{
    remove_feature_struct_member(inout.shaderDrawParameters, in.shaderDrawParameters);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDevice8BitStorageFeatures&    inout,
    const vk::PhysicalDeviceVulkan12Features& in
) -> void
{
    remove_feature_struct_member(
        inout.storageBuffer8BitAccess, in.storageBuffer8BitAccess
    );
    remove_feature_struct_member(
        inout.uniformAndStorageBuffer8BitAccess, in.uniformAndStorageBuffer8BitAccess
    );
    remove_feature_struct_member(inout.storagePushConstant8, in.storagePushConstant8);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderAtomicInt64Features& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    remove_feature_struct_member(
        inout.shaderBufferInt64Atomics, in.shaderBufferInt64Atomics
    );
    remove_feature_struct_member(
        inout.shaderSharedInt64Atomics, in.shaderSharedInt64Atomics
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderFloat16Int8Features& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    remove_feature_struct_member(inout.shaderFloat16, in.shaderFloat16);
    remove_feature_struct_member(inout.shaderInt8, in.shaderInt8);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceDescriptorIndexingFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&     in
) -> void
{
    remove_feature_struct_member(
        inout.shaderInputAttachmentArrayDynamicIndexing,
        in.shaderInputAttachmentArrayDynamicIndexing
    );
    remove_feature_struct_member(
        inout.shaderUniformTexelBufferArrayDynamicIndexing,
        in.shaderUniformTexelBufferArrayDynamicIndexing
    );
    remove_feature_struct_member(
        inout.shaderStorageTexelBufferArrayDynamicIndexing,
        in.shaderStorageTexelBufferArrayDynamicIndexing
    );
    remove_feature_struct_member(
        inout.shaderUniformBufferArrayNonUniformIndexing,
        in.shaderUniformBufferArrayNonUniformIndexing
    );
    remove_feature_struct_member(
        inout.shaderSampledImageArrayNonUniformIndexing,
        in.shaderSampledImageArrayNonUniformIndexing
    );
    remove_feature_struct_member(
        inout.shaderStorageBufferArrayNonUniformIndexing,
        in.shaderStorageBufferArrayNonUniformIndexing
    );
    remove_feature_struct_member(
        inout.shaderStorageImageArrayNonUniformIndexing,
        in.shaderStorageImageArrayNonUniformIndexing
    );
    remove_feature_struct_member(
        inout.shaderInputAttachmentArrayNonUniformIndexing,
        in.shaderInputAttachmentArrayNonUniformIndexing
    );
    remove_feature_struct_member(
        inout.shaderUniformTexelBufferArrayNonUniformIndexing,
        in.shaderUniformTexelBufferArrayNonUniformIndexing
    );
    remove_feature_struct_member(
        inout.shaderStorageTexelBufferArrayNonUniformIndexing,
        in.shaderStorageTexelBufferArrayNonUniformIndexing
    );
    remove_feature_struct_member(
        inout.descriptorBindingUniformBufferUpdateAfterBind,
        in.descriptorBindingUniformBufferUpdateAfterBind
    );
    remove_feature_struct_member(
        inout.descriptorBindingSampledImageUpdateAfterBind,
        in.descriptorBindingSampledImageUpdateAfterBind
    );
    remove_feature_struct_member(
        inout.descriptorBindingStorageImageUpdateAfterBind,
        in.descriptorBindingStorageImageUpdateAfterBind
    );
    remove_feature_struct_member(
        inout.descriptorBindingStorageBufferUpdateAfterBind,
        in.descriptorBindingStorageBufferUpdateAfterBind
    );
    remove_feature_struct_member(
        inout.descriptorBindingUniformTexelBufferUpdateAfterBind,
        in.descriptorBindingUniformTexelBufferUpdateAfterBind
    );
    remove_feature_struct_member(
        inout.descriptorBindingStorageTexelBufferUpdateAfterBind,
        in.descriptorBindingStorageTexelBufferUpdateAfterBind
    );
    remove_feature_struct_member(
        inout.descriptorBindingUpdateUnusedWhilePending,
        in.descriptorBindingUpdateUnusedWhilePending
    );
    remove_feature_struct_member(
        inout.descriptorBindingPartiallyBound, in.descriptorBindingPartiallyBound
    );
    remove_feature_struct_member(
        inout.descriptorBindingVariableDescriptorCount,
        in.descriptorBindingVariableDescriptorCount
    );
    remove_feature_struct_member(inout.runtimeDescriptorArray, in.runtimeDescriptorArray);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceScalarBlockLayoutFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    remove_feature_struct_member(inout.scalarBlockLayout, in.scalarBlockLayout);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceImagelessFramebufferFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&       in
) -> void
{
    remove_feature_struct_member(inout.imagelessFramebuffer, in.imagelessFramebuffer);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceUniformBufferStandardLayoutFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&              in
) -> void
{
    remove_feature_struct_member(
        inout.uniformBufferStandardLayout, in.uniformBufferStandardLayout
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderSubgroupExtendedTypesFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&              in
) -> void
{
    remove_feature_struct_member(
        inout.shaderSubgroupExtendedTypes, in.shaderSubgroupExtendedTypes
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceSeparateDepthStencilLayoutsFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&              in
) -> void
{
    remove_feature_struct_member(
        inout.separateDepthStencilLayouts, in.separateDepthStencilLayouts
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceHostQueryResetFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features& in
) -> void
{
    remove_feature_struct_member(inout.hostQueryReset, in.hostQueryReset);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceTimelineSemaphoreFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    remove_feature_struct_member(inout.timelineSemaphore, in.timelineSemaphore);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceBufferDeviceAddressFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&      in
) -> void
{
    remove_feature_struct_member(inout.bufferDeviceAddress, in.bufferDeviceAddress);
    remove_feature_struct_member(
        inout.bufferDeviceAddressCaptureReplay, in.bufferDeviceAddressCaptureReplay
    );
    remove_feature_struct_member(
        inout.bufferDeviceAddressMultiDevice, in.bufferDeviceAddressMultiDevice
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceVulkanMemoryModelFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    remove_feature_struct_member(inout.vulkanMemoryModel, in.vulkanMemoryModel);
    remove_feature_struct_member(
        inout.vulkanMemoryModelDeviceScope, in.vulkanMemoryModelDeviceScope
    );
    remove_feature_struct_member(
        inout.vulkanMemoryModelAvailabilityVisibilityChains,
        in.vulkanMemoryModelAvailabilityVisibilityChains
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceImageRobustnessFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&  in
) -> void
{
    remove_feature_struct_member(inout.robustImageAccess, in.robustImageAccess);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceInlineUniformBlockFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&     in
) -> void
{
    remove_feature_struct_member(inout.inlineUniformBlock, in.inlineUniformBlock);
    remove_feature_struct_member(
        inout.descriptorBindingInlineUniformBlockUpdateAfterBind,
        in.descriptorBindingInlineUniformBlockUpdateAfterBind
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDevicePipelineCreationCacheControlFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&               in
) -> void
{
    remove_feature_struct_member(
        inout.pipelineCreationCacheControl, in.pipelineCreationCacheControl
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDevicePrivateDataFeatures&    inout,
    const vk::PhysicalDeviceVulkan13Features& in
) -> void
{
    remove_feature_struct_member(inout.privateData, in.privateData);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderDemoteToHelperInvocationFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&                 in
) -> void
{
    remove_feature_struct_member(
        inout.shaderDemoteToHelperInvocation, in.shaderDemoteToHelperInvocation
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderTerminateInvocationFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&            in
) -> void
{
    remove_feature_struct_member(
        inout.shaderTerminateInvocation, in.shaderTerminateInvocation
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceSubgroupSizeControlFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&      in
) -> void
{
    remove_feature_struct_member(inout.subgroupSizeControl, in.subgroupSizeControl);
    remove_feature_struct_member(inout.computeFullSubgroups, in.computeFullSubgroups);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceSynchronization2Features& inout,
    const vk::PhysicalDeviceVulkan13Features&   in
) -> void
{
    remove_feature_struct_member(inout.synchronization2, in.synchronization2);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceTextureCompressionASTCHDRFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&            in
) -> void
{
    remove_feature_struct_member(
        inout.textureCompressionASTC_HDR, in.textureCompressionASTC_HDR
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceZeroInitializeWorkgroupMemoryFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&                in
) -> void
{
    remove_feature_struct_member(
        inout.shaderZeroInitializeWorkgroupMemory, in.shaderZeroInitializeWorkgroupMemory
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceDynamicRenderingFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&   in
) -> void
{
    remove_feature_struct_member(inout.dynamicRendering, in.dynamicRendering);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderIntegerDotProductFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&          in
) -> void
{
    remove_feature_struct_member(
        inout.shaderIntegerDotProduct, in.shaderIntegerDotProduct
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceMaintenance4Features&   inout,
    const vk::PhysicalDeviceVulkan13Features& in
) -> void
{
    remove_feature_struct_member(inout.maintenance4, in.maintenance4);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceGlobalPriorityQueryFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&      in
) -> void
{
    remove_feature_struct_member(inout.globalPriorityQuery, in.globalPriorityQuery);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderSubgroupRotateFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&       in
) -> void
{
    remove_feature_struct_member(inout.shaderSubgroupRotate, in.shaderSubgroupRotate);
    remove_feature_struct_member(
        inout.shaderSubgroupRotateClustered, in.shaderSubgroupRotateClustered
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderFloatControls2Features& inout,
    const vk::PhysicalDeviceVulkan14Features&       in
) -> void
{
    remove_feature_struct_member(inout.shaderFloatControls2, in.shaderFloatControls2);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceShaderExpectAssumeFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&     in
) -> void
{
    remove_feature_struct_member(inout.shaderExpectAssume, in.shaderExpectAssume);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceLineRasterizationFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&    in
) -> void
{
    remove_feature_struct_member(inout.rectangularLines, in.rectangularLines);
    remove_feature_struct_member(inout.bresenhamLines, in.bresenhamLines);
    remove_feature_struct_member(inout.smoothLines, in.smoothLines);
    remove_feature_struct_member(
        inout.stippledRectangularLines, in.stippledRectangularLines
    );
    remove_feature_struct_member(inout.stippledBresenhamLines, in.stippledBresenhamLines);
    remove_feature_struct_member(inout.stippledSmoothLines, in.stippledSmoothLines);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceVertexAttributeDivisorFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&         in
) -> void
{
    remove_feature_struct_member(
        inout.vertexAttributeInstanceRateDivisor, in.vertexAttributeInstanceRateDivisor
    );
    remove_feature_struct_member(
        inout.vertexAttributeInstanceRateZeroDivisor,
        in.vertexAttributeInstanceRateZeroDivisor
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceIndexTypeUint8Features& inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    remove_feature_struct_member(inout.indexTypeUint8, in.indexTypeUint8);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceDynamicRenderingLocalReadFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&            in
) -> void
{
    remove_feature_struct_member(
        inout.dynamicRenderingLocalRead, in.dynamicRenderingLocalRead
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceMaintenance5Features&   inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    remove_feature_struct_member(inout.maintenance5, in.maintenance5);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceMaintenance6Features&   inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    remove_feature_struct_member(inout.maintenance6, in.maintenance6);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDevicePipelineProtectedAccessFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&          in
) -> void
{
    remove_feature_struct_member(
        inout.pipelineProtectedAccess, in.pipelineProtectedAccess
    );
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDevicePipelineRobustnessFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&     in
) -> void
{
    remove_feature_struct_member(inout.pipelineRobustness, in.pipelineRobustness);
}

constexpr auto remove_physical_device_features_impl(
    vk::PhysicalDeviceHostImageCopyFeatures&  inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    remove_feature_struct_member(inout.hostImageCopy, in.hostImageCopy);
}

template <
    comprehensive_feature_struct_c ComprehensiveFeatureStruct_T,
    part_of_comprehensive_feature_struct_c<ComprehensiveFeatureStruct_T>
        IndividualFeatureStruct_T>
constexpr auto remove_physical_device_features(
    IndividualFeatureStruct_T&          inout,
    const ComprehensiveFeatureStruct_T& in
) -> void
{
    remove_physical_device_features_impl(inout, in);
}

}   // namespace ddge::vulkan
