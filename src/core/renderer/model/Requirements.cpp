#include "Requirements.hpp"

// Required extensions:
//     - VK_KHR_buffer_device_address

namespace core::renderer {

auto RenderModel::Requirements::required_instance_settings_are_available(
    const vkb::SystemInfo& t_system_info
) -> bool
{
    return t_system_info.is_extension_available(
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    );
}

auto RenderModel::Requirements::enable_instance_settings(
    const vkb::SystemInfo&,
    vkb::InstanceBuilder& t_instance_builder
) -> void
{
    t_instance_builder.enable_extension(
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    );
}

auto RenderModel::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& t_physical_device_selector
) -> void
{
    t_physical_device_selector.add_required_extension(VK_KHR_DEVICE_GROUP_EXTENSION_NAME);
    t_physical_device_selector.add_required_extension(
        VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
    );
    constexpr static vk::PhysicalDeviceBufferDeviceAddressFeatures
        buffer_device_address_features{ .bufferDeviceAddress = true };
    t_physical_device_selector.add_required_extension_features(
        buffer_device_address_features
    );
}

auto RenderModel::Requirements::enable_optional_device_settings(vkb::PhysicalDevice&)
    -> void
{}

}   // namespace core::renderer
