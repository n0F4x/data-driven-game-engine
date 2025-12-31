export module ddge.modules.vulkan.extension_inspection.try_promote_extension_to_vulkan1x;

import vulkan_hpp;

import ddge.modules.vulkan.extension_inspection.extension_is_promoted_to_vulkan1x;
import ddge.utility.containers.StringLiteral;

namespace ddge::vulkan {

/*
 * See extension to feature promotions at
 * https://docs.vulkan.org/spec/latest/chapters/features.html
 * - Table 1. Extension Feature Aliases
 */

export [[nodiscard]]
auto try_promote_extension_to_vulkan11(
    util::StringLiteral                 extension_name,
    vk::PhysicalDeviceVulkan11Features& vulkan11_features
) -> bool;

export [[nodiscard]]
auto try_promote_extension_to_vulkan12(
    util::StringLiteral                 extension_name,
    vk::PhysicalDeviceVulkan12Features& vulkan12_features
) -> bool;

export [[nodiscard]]
auto try_promote_extension_to_vulkan13(
    util::StringLiteral                 extension_name,
    vk::PhysicalDeviceVulkan13Features& vulkan13_features
) -> bool;

export [[nodiscard]]
auto try_promote_extension_to_vulkan14(
    util::StringLiteral                 extension_name,
    vk::PhysicalDeviceVulkan14Features& vulkan14_features
) -> bool;

}   // namespace ddge::vulkan

module :private;

namespace ddge::vulkan {

auto try_promote_extension_to_vulkan11(
    const util::StringLiteral           extension_name,
    vk::PhysicalDeviceVulkan11Features& vulkan11_features
) -> bool
{
    if (!extension_is_promoted_to_vulkan11(extension_name)) {
        return false;
    }

    if (extension_name == util::StringLiteral{ vk::KHRShaderDrawParametersExtensionName })
    {
        vulkan11_features.shaderDrawParameters = vk::True;
        return true;
    }

    return true;
}

auto try_promote_extension_to_vulkan12(
    const util::StringLiteral           extension_name,
    vk::PhysicalDeviceVulkan12Features& vulkan12_features
) -> bool
{
    if (!extension_is_promoted_to_vulkan12(extension_name)) {
        return false;
    }

    if (extension_name
        == util::StringLiteral{ vk::KHRSamplerMirrorClampToEdgeExtensionName })
    {
        vulkan12_features.samplerMirrorClampToEdge = vk::True;
        return true;
    }

    if (extension_name == util::StringLiteral{ vk::KHRDrawIndirectCountExtensionName }) {
        vulkan12_features.drawIndirectCount = vk::True;
        return true;
    }

    if (extension_name == util::StringLiteral{ vk::EXTDescriptorIndexingExtensionName }) {
        vulkan12_features.descriptorIndexing = vk::True;
        return true;
    }

    if (extension_name == util::StringLiteral{ vk::EXTSamplerFilterMinmaxExtensionName })
    {
        vulkan12_features.samplerFilterMinmax = vk::True;
        return true;
    }

    if (extension_name
        == util::StringLiteral{ vk::EXTShaderViewportIndexLayerExtensionName })
    {
        vulkan12_features.shaderOutputViewportIndex = vk::True;
        vulkan12_features.shaderOutputLayer         = vk::True;
        return true;
    }

    return true;
}

auto try_promote_extension_to_vulkan13(
    const util::StringLiteral extension_name,
    vk::PhysicalDeviceVulkan13Features&
) -> bool
{
    return extension_is_promoted_to_vulkan13(extension_name);
}

auto try_promote_extension_to_vulkan14(
    const util::StringLiteral           extension_name,
    vk::PhysicalDeviceVulkan14Features& vulkan14_features
) -> bool
{
    if (!extension_is_promoted_to_vulkan14(extension_name)) {
        return false;
    }

    if (extension_name == util::StringLiteral{ vk::KHRPushDescriptorExtensionName }) {
        vulkan14_features.pushDescriptor = vk::True;
        return true;
    }

    return true;
}

}   // namespace ddge::vulkan
