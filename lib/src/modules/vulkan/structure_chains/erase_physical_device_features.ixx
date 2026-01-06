export module ddge.modules.vulkan.structure_chains.erase_physical_device_features;

import vulkan_hpp;

import ddge.modules.vulkan.structure_chains.comprehensive_feature_struct_c;
import ddge.modules.vulkan.structure_chains.feature_struct_c;
import ddge.modules.vulkan.structure_chains.part_of_comprehensive_feature_struct_c;

namespace ddge::vulkan {

export template <feature_struct_c FeatureStruct_T>
constexpr auto
    erase_physical_device_features(FeatureStruct_T& inout, const FeatureStruct_T& in)
        -> void;

export template <
    comprehensive_feature_struct_c ComprehensiveFeatureStruct_T,
    part_of_comprehensive_feature_struct_c<ComprehensiveFeatureStruct_T>
        IndividualFeatureStruct_T>
constexpr auto erase_physical_device_features(
    IndividualFeatureStruct_T&          inout,
    const ComprehensiveFeatureStruct_T& in
) -> void;

}   // namespace ddge::vulkan

namespace ddge::vulkan {

constexpr auto
    erase_feature_struct_member(const vk::StructureType, const vk::StructureType) -> void
{}

constexpr auto erase_feature_struct_member(void* const, void* const) -> void {}

constexpr auto erase_feature_struct_member(vk::Bool32& inout, const vk::Bool32 in)
    -> void
{
    if (in) {
        inout = vk::False;
    }
}

template <feature_struct_c FeatureStruct_T>
constexpr auto
    erase_physical_device_features(FeatureStruct_T& inout, const FeatureStruct_T& in)
        -> void
{
    auto& [... inout_members]{ inout };
    const auto& [... in_members]{ in };
    (erase_feature_struct_member(inout_members, in_members), ...);
}

// TODO: use reflection

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDevice16BitStorageFeatures&   inout,
    const vk::PhysicalDeviceVulkan11Features& in
) -> void
{
    erase_feature_struct_member(
        inout.storageBuffer16BitAccess, in.storageBuffer16BitAccess
    );
    erase_feature_struct_member(
        inout.uniformAndStorageBuffer16BitAccess, in.uniformAndStorageBuffer16BitAccess
    );
    erase_feature_struct_member(inout.storagePushConstant16, in.storagePushConstant16);
    erase_feature_struct_member(inout.storageInputOutput16, in.storageInputOutput16);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceMultiviewFeatures&      inout,
    const vk::PhysicalDeviceVulkan11Features& in
) -> void
{
    erase_feature_struct_member(inout.multiview, in.multiview);
    erase_feature_struct_member(
        inout.multiviewGeometryShader, in.multiviewGeometryShader
    );
    erase_feature_struct_member(
        inout.multiviewTessellationShader, in.multiviewTessellationShader
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceVariablePointersFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&   in
) -> void
{
    erase_feature_struct_member(
        inout.variablePointersStorageBuffer, in.variablePointersStorageBuffer
    );
    erase_feature_struct_member(inout.variablePointers, in.variablePointers);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceProtectedMemoryFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&  in
) -> void
{
    erase_feature_struct_member(inout.protectedMemory, in.protectedMemory);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceSamplerYcbcrConversionFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&         in
) -> void
{
    erase_feature_struct_member(inout.samplerYcbcrConversion, in.samplerYcbcrConversion);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderDrawParametersFeatures& inout,
    const vk::PhysicalDeviceVulkan11Features&       in
) -> void
{
    erase_feature_struct_member(inout.shaderDrawParameters, in.shaderDrawParameters);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDevice8BitStorageFeatures&    inout,
    const vk::PhysicalDeviceVulkan12Features& in
) -> void
{
    erase_feature_struct_member(
        inout.storageBuffer8BitAccess, in.storageBuffer8BitAccess
    );
    erase_feature_struct_member(
        inout.uniformAndStorageBuffer8BitAccess, in.uniformAndStorageBuffer8BitAccess
    );
    erase_feature_struct_member(inout.storagePushConstant8, in.storagePushConstant8);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderAtomicInt64Features& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    erase_feature_struct_member(
        inout.shaderBufferInt64Atomics, in.shaderBufferInt64Atomics
    );
    erase_feature_struct_member(
        inout.shaderSharedInt64Atomics, in.shaderSharedInt64Atomics
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderFloat16Int8Features& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    erase_feature_struct_member(inout.shaderFloat16, in.shaderFloat16);
    erase_feature_struct_member(inout.shaderInt8, in.shaderInt8);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceDescriptorIndexingFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&     in
) -> void
{
    erase_feature_struct_member(
        inout.shaderInputAttachmentArrayDynamicIndexing,
        in.shaderInputAttachmentArrayDynamicIndexing
    );
    erase_feature_struct_member(
        inout.shaderUniformTexelBufferArrayDynamicIndexing,
        in.shaderUniformTexelBufferArrayDynamicIndexing
    );
    erase_feature_struct_member(
        inout.shaderStorageTexelBufferArrayDynamicIndexing,
        in.shaderStorageTexelBufferArrayDynamicIndexing
    );
    erase_feature_struct_member(
        inout.shaderUniformBufferArrayNonUniformIndexing,
        in.shaderUniformBufferArrayNonUniformIndexing
    );
    erase_feature_struct_member(
        inout.shaderSampledImageArrayNonUniformIndexing,
        in.shaderSampledImageArrayNonUniformIndexing
    );
    erase_feature_struct_member(
        inout.shaderStorageBufferArrayNonUniformIndexing,
        in.shaderStorageBufferArrayNonUniformIndexing
    );
    erase_feature_struct_member(
        inout.shaderStorageImageArrayNonUniformIndexing,
        in.shaderStorageImageArrayNonUniformIndexing
    );
    erase_feature_struct_member(
        inout.shaderInputAttachmentArrayNonUniformIndexing,
        in.shaderInputAttachmentArrayNonUniformIndexing
    );
    erase_feature_struct_member(
        inout.shaderUniformTexelBufferArrayNonUniformIndexing,
        in.shaderUniformTexelBufferArrayNonUniformIndexing
    );
    erase_feature_struct_member(
        inout.shaderStorageTexelBufferArrayNonUniformIndexing,
        in.shaderStorageTexelBufferArrayNonUniformIndexing
    );
    erase_feature_struct_member(
        inout.descriptorBindingUniformBufferUpdateAfterBind,
        in.descriptorBindingUniformBufferUpdateAfterBind
    );
    erase_feature_struct_member(
        inout.descriptorBindingSampledImageUpdateAfterBind,
        in.descriptorBindingSampledImageUpdateAfterBind
    );
    erase_feature_struct_member(
        inout.descriptorBindingStorageImageUpdateAfterBind,
        in.descriptorBindingStorageImageUpdateAfterBind
    );
    erase_feature_struct_member(
        inout.descriptorBindingStorageBufferUpdateAfterBind,
        in.descriptorBindingStorageBufferUpdateAfterBind
    );
    erase_feature_struct_member(
        inout.descriptorBindingUniformTexelBufferUpdateAfterBind,
        in.descriptorBindingUniformTexelBufferUpdateAfterBind
    );
    erase_feature_struct_member(
        inout.descriptorBindingStorageTexelBufferUpdateAfterBind,
        in.descriptorBindingStorageTexelBufferUpdateAfterBind
    );
    erase_feature_struct_member(
        inout.descriptorBindingUpdateUnusedWhilePending,
        in.descriptorBindingUpdateUnusedWhilePending
    );
    erase_feature_struct_member(
        inout.descriptorBindingPartiallyBound, in.descriptorBindingPartiallyBound
    );
    erase_feature_struct_member(
        inout.descriptorBindingVariableDescriptorCount,
        in.descriptorBindingVariableDescriptorCount
    );
    erase_feature_struct_member(inout.runtimeDescriptorArray, in.runtimeDescriptorArray);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceScalarBlockLayoutFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    erase_feature_struct_member(inout.scalarBlockLayout, in.scalarBlockLayout);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceImagelessFramebufferFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&       in
) -> void
{
    erase_feature_struct_member(inout.imagelessFramebuffer, in.imagelessFramebuffer);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceUniformBufferStandardLayoutFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&              in
) -> void
{
    erase_feature_struct_member(
        inout.uniformBufferStandardLayout, in.uniformBufferStandardLayout
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderSubgroupExtendedTypesFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&              in
) -> void
{
    erase_feature_struct_member(
        inout.shaderSubgroupExtendedTypes, in.shaderSubgroupExtendedTypes
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceSeparateDepthStencilLayoutsFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&              in
) -> void
{
    erase_feature_struct_member(
        inout.separateDepthStencilLayouts, in.separateDepthStencilLayouts
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceHostQueryResetFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features& in
) -> void
{
    erase_feature_struct_member(inout.hostQueryReset, in.hostQueryReset);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceTimelineSemaphoreFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    erase_feature_struct_member(inout.timelineSemaphore, in.timelineSemaphore);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceBufferDeviceAddressFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&      in
) -> void
{
    erase_feature_struct_member(inout.bufferDeviceAddress, in.bufferDeviceAddress);
    erase_feature_struct_member(
        inout.bufferDeviceAddressCaptureReplay, in.bufferDeviceAddressCaptureReplay
    );
    erase_feature_struct_member(
        inout.bufferDeviceAddressMultiDevice, in.bufferDeviceAddressMultiDevice
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceVulkanMemoryModelFeatures& inout,
    const vk::PhysicalDeviceVulkan12Features&    in
) -> void
{
    erase_feature_struct_member(inout.vulkanMemoryModel, in.vulkanMemoryModel);
    erase_feature_struct_member(
        inout.vulkanMemoryModelDeviceScope, in.vulkanMemoryModelDeviceScope
    );
    erase_feature_struct_member(
        inout.vulkanMemoryModelAvailabilityVisibilityChains,
        in.vulkanMemoryModelAvailabilityVisibilityChains
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceImageRobustnessFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&  in
) -> void
{
    erase_feature_struct_member(inout.robustImageAccess, in.robustImageAccess);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceInlineUniformBlockFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&     in
) -> void
{
    erase_feature_struct_member(inout.inlineUniformBlock, in.inlineUniformBlock);
    erase_feature_struct_member(
        inout.descriptorBindingInlineUniformBlockUpdateAfterBind,
        in.descriptorBindingInlineUniformBlockUpdateAfterBind
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDevicePipelineCreationCacheControlFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&               in
) -> void
{
    erase_feature_struct_member(
        inout.pipelineCreationCacheControl, in.pipelineCreationCacheControl
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDevicePrivateDataFeatures&    inout,
    const vk::PhysicalDeviceVulkan13Features& in
) -> void
{
    erase_feature_struct_member(inout.privateData, in.privateData);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderDemoteToHelperInvocationFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&                 in
) -> void
{
    erase_feature_struct_member(
        inout.shaderDemoteToHelperInvocation, in.shaderDemoteToHelperInvocation
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderTerminateInvocationFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&            in
) -> void
{
    erase_feature_struct_member(
        inout.shaderTerminateInvocation, in.shaderTerminateInvocation
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceSubgroupSizeControlFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&      in
) -> void
{
    erase_feature_struct_member(inout.subgroupSizeControl, in.subgroupSizeControl);
    erase_feature_struct_member(inout.computeFullSubgroups, in.computeFullSubgroups);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceSynchronization2Features& inout,
    const vk::PhysicalDeviceVulkan13Features&   in
) -> void
{
    erase_feature_struct_member(inout.synchronization2, in.synchronization2);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceTextureCompressionASTCHDRFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&            in
) -> void
{
    erase_feature_struct_member(
        inout.textureCompressionASTC_HDR, in.textureCompressionASTC_HDR
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceZeroInitializeWorkgroupMemoryFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&                in
) -> void
{
    erase_feature_struct_member(
        inout.shaderZeroInitializeWorkgroupMemory, in.shaderZeroInitializeWorkgroupMemory
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceDynamicRenderingFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&   in
) -> void
{
    erase_feature_struct_member(inout.dynamicRendering, in.dynamicRendering);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderIntegerDotProductFeatures& inout,
    const vk::PhysicalDeviceVulkan13Features&          in
) -> void
{
    erase_feature_struct_member(
        inout.shaderIntegerDotProduct, in.shaderIntegerDotProduct
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceMaintenance4Features&   inout,
    const vk::PhysicalDeviceVulkan13Features& in
) -> void
{
    erase_feature_struct_member(inout.maintenance4, in.maintenance4);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceGlobalPriorityQueryFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&      in
) -> void
{
    erase_feature_struct_member(inout.globalPriorityQuery, in.globalPriorityQuery);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderSubgroupRotateFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&       in
) -> void
{
    erase_feature_struct_member(inout.shaderSubgroupRotate, in.shaderSubgroupRotate);
    erase_feature_struct_member(
        inout.shaderSubgroupRotateClustered, in.shaderSubgroupRotateClustered
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderFloatControls2Features& inout,
    const vk::PhysicalDeviceVulkan14Features&       in
) -> void
{
    erase_feature_struct_member(inout.shaderFloatControls2, in.shaderFloatControls2);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceShaderExpectAssumeFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&     in
) -> void
{
    erase_feature_struct_member(inout.shaderExpectAssume, in.shaderExpectAssume);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceLineRasterizationFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&    in
) -> void
{
    erase_feature_struct_member(inout.rectangularLines, in.rectangularLines);
    erase_feature_struct_member(inout.bresenhamLines, in.bresenhamLines);
    erase_feature_struct_member(inout.smoothLines, in.smoothLines);
    erase_feature_struct_member(
        inout.stippledRectangularLines, in.stippledRectangularLines
    );
    erase_feature_struct_member(inout.stippledBresenhamLines, in.stippledBresenhamLines);
    erase_feature_struct_member(inout.stippledSmoothLines, in.stippledSmoothLines);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceVertexAttributeDivisorFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&         in
) -> void
{
    erase_feature_struct_member(
        inout.vertexAttributeInstanceRateDivisor, in.vertexAttributeInstanceRateDivisor
    );
    erase_feature_struct_member(
        inout.vertexAttributeInstanceRateZeroDivisor,
        in.vertexAttributeInstanceRateZeroDivisor
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceIndexTypeUint8Features& inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    erase_feature_struct_member(inout.indexTypeUint8, in.indexTypeUint8);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceDynamicRenderingLocalReadFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&            in
) -> void
{
    erase_feature_struct_member(
        inout.dynamicRenderingLocalRead, in.dynamicRenderingLocalRead
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceMaintenance5Features&   inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    erase_feature_struct_member(inout.maintenance5, in.maintenance5);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceMaintenance6Features&   inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    erase_feature_struct_member(inout.maintenance6, in.maintenance6);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDevicePipelineProtectedAccessFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&          in
) -> void
{
    erase_feature_struct_member(
        inout.pipelineProtectedAccess, in.pipelineProtectedAccess
    );
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDevicePipelineRobustnessFeatures& inout,
    const vk::PhysicalDeviceVulkan14Features&     in
) -> void
{
    erase_feature_struct_member(inout.pipelineRobustness, in.pipelineRobustness);
}

constexpr auto erase_physical_device_features_impl(
    vk::PhysicalDeviceHostImageCopyFeatures&  inout,
    const vk::PhysicalDeviceVulkan14Features& in
) -> void
{
    erase_feature_struct_member(inout.hostImageCopy, in.hostImageCopy);
}

template <
    comprehensive_feature_struct_c ComprehensiveFeatureStruct_T,
    part_of_comprehensive_feature_struct_c<ComprehensiveFeatureStruct_T>
        IndividualFeatureStruct_T>
constexpr auto erase_physical_device_features(
    IndividualFeatureStruct_T&          inout,
    const ComprehensiveFeatureStruct_T& in
) -> void
{
    erase_physical_device_features_impl(inout, in);
}

}   // namespace ddge::vulkan
