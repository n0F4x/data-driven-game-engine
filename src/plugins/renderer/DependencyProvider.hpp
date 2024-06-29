#pragma once

#include <VkBootstrap.h>

namespace plugins::renderer {

class DependencyProvider {
public:
    virtual ~DependencyProvider() = default;

    [[nodiscard]]
    virtual auto
        required_instance_settings_are_available(const vkb::SystemInfo& t_system_info
        ) -> bool = 0;

    virtual auto enable_instance_settings(
        const vkb::SystemInfo& t_system_info,
        vkb::InstanceBuilder&  t_builder
    ) -> void = 0;

    virtual auto
        require_device_settings(vkb::PhysicalDeviceSelector& t_physical_device_selector
        ) -> void = 0;

    virtual auto enable_optional_device_settings(vkb::PhysicalDevice& t_physical_device
    ) -> void = 0;
};

}   // namespace plugins::renderer
