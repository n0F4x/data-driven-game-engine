#include "Requirements.hpp"

// Required extensions:
//     - VK_KHR_buffer_device_address
//     - VK_EXT_descriptor_indexing

namespace core::renderer {

auto ModelLayout::Requirements::required_instance_settings_are_available(
    const vkb::SystemInfo& system_info
) -> bool
{
    return system_info.is_extension_available(
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    );
}

auto ModelLayout::Requirements::enable_instance_settings(
    const vkb::SystemInfo&,
    vkb::InstanceBuilder& instance_builder
) -> void
{
    instance_builder.enable_extension(
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    );
}

auto ModelLayout::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& physical_device_selector
) -> void
{
    // VK_KHR_buffer_device_address
    physical_device_selector.add_required_extension(VK_KHR_DEVICE_GROUP_EXTENSION_NAME);
    physical_device_selector.add_required_extension(
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
    );
    physical_device_selector.add_required_extension_features(
        vk::PhysicalDeviceBufferDeviceAddressFeatures{ .bufferDeviceAddress = vk::True }
    );

    // VK_EXT_descriptor_indexing
    physical_device_selector.add_required_extension(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
    physical_device_selector.add_required_extension(
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
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
