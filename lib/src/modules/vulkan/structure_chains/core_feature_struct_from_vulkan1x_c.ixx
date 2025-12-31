module;

#include <concepts>

export module ddge.modules.vulkan.structure_chains.core_feature_struct_from_vulkan1x_c;

import vulkan_hpp;

namespace ddge::vulkan {

export template <typename T>
concept core_feature_struct_from_vulkan11_c =
    std::same_as<T, vk::PhysicalDevice16BitStorageFeatures>
    || std::same_as<T, vk::PhysicalDeviceMultiviewFeatures>
    || std::same_as<T, vk::PhysicalDeviceVariablePointersFeatures>
    || std::same_as<T, vk::PhysicalDeviceProtectedMemoryFeatures>
    || std::same_as<T, vk::PhysicalDeviceSamplerYcbcrConversionFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderDrawParametersFeatures>;

export template <typename T>
concept core_feature_struct_from_vulkan12_c =
    std::same_as<T, vk::PhysicalDevice8BitStorageFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderAtomicInt64Features>
    || std::same_as<T, vk::PhysicalDeviceShaderFloat16Int8Features>
    || std::same_as<T, vk::PhysicalDeviceDescriptorIndexingFeatures>
    || std::same_as<T, vk::PhysicalDeviceScalarBlockLayoutFeatures>
    || std::same_as<T, vk::PhysicalDeviceImagelessFramebufferFeatures>
    || std::same_as<T, vk::PhysicalDeviceUniformBufferStandardLayoutFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderSubgroupExtendedTypesFeatures>
    || std::same_as<T, vk::PhysicalDeviceSeparateDepthStencilLayoutsFeatures>
    || std::same_as<T, vk::PhysicalDeviceHostQueryResetFeatures>
    || std::same_as<T, vk::PhysicalDeviceTimelineSemaphoreFeatures>
    || std::same_as<T, vk::PhysicalDeviceBufferDeviceAddressFeatures>
    || std::same_as<T, vk::PhysicalDeviceVulkanMemoryModelFeatures>;

export template <typename T>
concept core_feature_struct_from_vulkan13_c =
    std::same_as<T, vk::PhysicalDeviceImageRobustnessFeatures>
    || std::same_as<T, vk::PhysicalDeviceInlineUniformBlockFeatures>
    || std::same_as<T, vk::PhysicalDevicePipelineCreationCacheControlFeatures>
    || std::same_as<T, vk::PhysicalDevicePrivateDataFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderDemoteToHelperInvocationFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderTerminateInvocationFeatures>
    || std::same_as<T, vk::PhysicalDeviceSubgroupSizeControlFeatures>
    || std::same_as<T, vk::PhysicalDeviceSynchronization2Features>
    || std::same_as<T, vk::PhysicalDeviceTextureCompressionASTCHDRFeatures>
    || std::same_as<T, vk::PhysicalDeviceZeroInitializeWorkgroupMemoryFeatures>
    || std::same_as<T, vk::PhysicalDeviceDynamicRenderingFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderIntegerDotProductFeatures>
    || std::same_as<T, vk::PhysicalDeviceMaintenance4Features>;

export template <typename T>
concept core_feature_struct_from_vulkan14_c =
    std::same_as<T, vk::PhysicalDeviceGlobalPriorityQueryFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderSubgroupRotateFeatures>
    || std::same_as<T, vk::PhysicalDeviceShaderFloatControls2Features>
    || std::same_as<T, vk::PhysicalDeviceShaderExpectAssumeFeatures>
    || std::same_as<T, vk::PhysicalDeviceLineRasterizationFeatures>
    || std::same_as<T, vk::PhysicalDeviceVertexAttributeDivisorFeatures>
    || std::same_as<T, vk::PhysicalDeviceIndexTypeUint8Features>
    || std::same_as<T, vk::PhysicalDeviceDynamicRenderingLocalReadFeatures>
    || std::same_as<T, vk::PhysicalDeviceMaintenance5Features>
    || std::same_as<T, vk::PhysicalDeviceMaintenance6Features>
    || std::same_as<T, vk::PhysicalDevicePipelineProtectedAccessFeatures>
    || std::same_as<T, vk::PhysicalDevicePipelineRobustnessFeatures>
    || std::same_as<T, vk::PhysicalDeviceHostImageCopyFeatures>;

export template <typename T>
concept core_feature_struct_from_vulkan1x_c = core_feature_struct_from_vulkan11_c<T>
                             || core_feature_struct_from_vulkan12_c<T>
                             || core_feature_struct_from_vulkan13_c<T>
                             || core_feature_struct_from_vulkan14_c<T>;

}   // namespace ddge::vulkan
