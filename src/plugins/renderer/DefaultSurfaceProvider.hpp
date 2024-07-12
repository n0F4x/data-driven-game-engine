#pragma once

#include "app/App.hpp"
#include "core/window/Window.hpp"
#include "store/Store.hpp"

#include "DependencyProvider.hpp"

namespace plugins::renderer {

class DefaultSurfaceProvider : public renderer::DependencyProvider {
public:
    [[nodiscard]]
    auto operator()(
        const App&                   app,
        VkInstance                   instance,
        const VkAllocationCallbacks* allocation_callbacks
    ) const -> std::optional<VkSurfaceKHR>;

    [[nodiscard]]
    auto required_instance_settings_are_available(const vkb::SystemInfo& t_system_info
    ) -> bool override;

    auto enable_instance_settings(
        const vkb::SystemInfo& t_system_info,
        vkb::InstanceBuilder&  t_builder
    ) -> void override;

    auto require_device_settings(vkb::PhysicalDeviceSelector& t_physical_device_selector
    ) -> void override;

    auto enable_optional_device_settings(vkb::PhysicalDevice& t_physical_device
    ) -> void override;

    [[nodiscard]]
    static auto dependencies() -> std::span<const std::type_index>;
};

}   // namespace plugins::renderer
