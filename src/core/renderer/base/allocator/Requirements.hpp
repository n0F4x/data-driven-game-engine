#pragma once

#include "Allocator.hpp"

namespace core::renderer {

class Allocator::Requirements {
public:
    [[nodiscard]] static auto
        required_instance_settings_are_available(const vkb::SystemInfo& t_system_info
        ) -> bool;

    static auto enable_instance_settings(
        const vkb::SystemInfo& t_system_info,
        vkb::InstanceBuilder&  t_builder
    ) -> void;

    static auto
        require_device_settings(vkb::PhysicalDeviceSelector& t_physical_device_selector
        ) -> void;

    static auto enable_optional_device_settings(vkb::PhysicalDevice& t_physical_device) -> void;

    [[deprecated("The current setup doesn't support setting these options"
    )]] [[nodiscard]] static auto
        optional_device_extension_structs(const vkb::PhysicalDevice& t_physical_device)
            -> vk::StructureChain<
                vk::DeviceCreateInfo,
                vk::PhysicalDeviceCoherentMemoryFeaturesAMD,
                vk::PhysicalDeviceBufferDeviceAddressFeatures,
                vk::PhysicalDeviceMemoryPriorityFeaturesEXT,
                vk::PhysicalDeviceMaintenance4Features>;
};

}   // namespace core::renderer
