#pragma once

#include <VkBootstrap.h>

#include "Allocator.hpp"

namespace core::renderer::base {

class Allocator::Requirements {
public:
    [[nodiscard]]
    static auto required_instance_settings_are_available(const vkb::SystemInfo& system_info
    ) -> bool;

    static auto enable_instance_settings(
        const vkb::SystemInfo& system_info,
        vkb::InstanceBuilder&  builder
    ) -> void;

    static auto
        require_device_settings(vkb::PhysicalDeviceSelector& physical_device_selector)
            -> void;

    static auto enable_optional_device_settings(vkb::PhysicalDevice& physical_device)
        -> void;
};

}   // namespace core::renderer
