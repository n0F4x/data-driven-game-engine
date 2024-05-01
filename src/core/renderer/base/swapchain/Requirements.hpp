#pragma once

#include "Swapchain.hpp"

namespace core::renderer {

class Swapchain::Requirements {
public:
    [[nodiscard]]
    static auto
        required_instance_settings_are_available(const vkb::SystemInfo& t_system_info
        ) -> bool;

    static auto enable_instance_settings(
        const vkb::SystemInfo& t_system_info,
        vkb::InstanceBuilder&  t_builder
    ) -> void;

    static auto
        require_device_settings(vkb::PhysicalDeviceSelector& t_physical_device_selector
        ) -> void;

    static auto enable_optional_device_settings(vkb::PhysicalDevice& t_physical_device
    ) -> void;
};

}   // namespace core::renderer
