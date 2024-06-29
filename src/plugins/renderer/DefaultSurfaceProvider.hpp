#pragma once

#include "core/window/Window.hpp"
#include "store/Store.hpp"

#include "DependencyProvider.hpp"

namespace plugins::renderer {

class DefaultSurfaceProvider : public renderer::DependencyProvider {
public:
    explicit DefaultSurfaceProvider(const core::window::Window& window) noexcept;

    [[nodiscard]]
    auto operator()(
        VkInstance                   t_instance,
        const VkAllocationCallbacks* t_allocation_callbacks
    ) const -> std::expected<VkSurfaceKHR, VkResult>;

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

private:
    std::reference_wrapper<const core::window::Window> m_window;
};

}   // namespace plugins::renderer
