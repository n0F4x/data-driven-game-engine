module;



#include <VkBootstrap.h>

#include <gsl-lite/gsl-lite.hpp>

module core.renderer.base.allocator.Allocator;

namespace core::renderer::base {

[[nodiscard]]
constexpr static auto optional_instance_extension_names()
    -> const std::array<gsl_lite::not_null_ic<gsl_lite::czstring>, 5>&
{
    constexpr static std::array<gsl_lite::not_null_ic<gsl_lite::czstring>, 5>
        s_extension_names{ // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
                           vk::KHRGetPhysicalDeviceProperties2ExtensionName,

                           // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
                           vk::KHRGetPhysicalDeviceProperties2ExtensionName,

                           // VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT
                           vk::KHRGetPhysicalDeviceProperties2ExtensionName,
                           vk::KHRDeviceGroupCreationExtensionName,

                           // VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT
                           vk::KHRGetPhysicalDeviceProperties2ExtensionName
        };

    return s_extension_names;
}

auto Allocator::Requirements::required_instance_settings_are_available(const vkb::SystemInfo&)
    -> bool
{
    return true;
}

auto Allocator::Requirements::enable_instance_settings(
    const vkb::SystemInfo& system_info,
    vkb::InstanceBuilder&  builder
) -> void
{
    for (const auto& extension_name : optional_instance_extension_names()) {
        if (system_info.is_extension_available(extension_name)) {
            builder.enable_extension(extension_name);
        }
    }
}

auto Allocator::Requirements::require_device_settings(vkb::PhysicalDeviceSelector&)
    -> void
{}

auto Allocator::Requirements::enable_optional_device_settings(
    vkb::PhysicalDevice& physical_device
) -> void
{
    // VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT
    physical_device.enable_extension_if_present(vk::KHRGetMemoryRequirements2ExtensionName
    );
    physical_device.enable_extension_if_present(vk::KHRDedicatedAllocationExtensionName);

    // VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT
    physical_device.enable_extension_if_present(vk::KHRBindMemory2ExtensionName);

    // VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
    physical_device.enable_extension_if_present(vk::EXTMemoryBudgetExtensionName);

    // VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
    physical_device.enable_extension_if_present(vk::AMDDeviceCoherentMemoryExtensionName);
    constexpr static vk::PhysicalDeviceCoherentMemoryFeaturesAMD
        coherent_memory_features_AMD{ .deviceCoherentMemory = vk::True };
    physical_device.enable_extension_features_if_present(coherent_memory_features_AMD);
}

}   // namespace core::renderer::base
