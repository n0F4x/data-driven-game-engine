module;


#include <VkBootstrap.h>

module core.renderer.model.ModelLayout;

// Required extensions:
//     - VK_KHR_buffer_device_address
//     - VK_EXT_descriptor_indexing

namespace core::renderer {

auto ModelLayout::Requirements::required_instance_settings_are_available(
    const vkb::SystemInfo& system_info
) -> bool
{
    return system_info.is_extension_available(
        vk::KHRGetPhysicalDeviceProperties2ExtensionName
    );
}

auto ModelLayout::Requirements::enable_instance_settings(
    const vkb::SystemInfo&,
    vkb::InstanceBuilder& instance_builder
) -> void
{
    instance_builder.enable_extension(vk::KHRGetPhysicalDeviceProperties2ExtensionName);
}

auto ModelLayout::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& physical_device_selector
) -> void
{
    // VK_KHR_buffer_device_address
    physical_device_selector.add_required_extension(vk::KHRDeviceGroupExtensionName);
    physical_device_selector.add_required_extension(
        vk::KHRBufferDeviceAddressExtensionName
    );
    physical_device_selector.add_required_extension_features(
        vk::PhysicalDeviceBufferDeviceAddressFeatures{ .bufferDeviceAddress = vk::True }
    );

    // VK_EXT_descriptor_indexing
    physical_device_selector.add_required_extension(vk::KHRMaintenance3ExtensionName);
    physical_device_selector.add_required_extension(
        vk::EXTDescriptorIndexingExtensionName
    );
    physical_device_selector.add_required_extension_features(
        vk::PhysicalDeviceDescriptorIndexingFeatures{
            .shaderSampledImageArrayNonUniformIndexing = vk::True,
            .descriptorBindingVariableDescriptorCount  = vk::True,
            .runtimeDescriptorArray                    = vk::True,
        }
    );
}

auto ModelLayout::Requirements::enable_optional_device_settings(vkb::PhysicalDevice&)
    -> void
{}

}   // namespace core::renderer
