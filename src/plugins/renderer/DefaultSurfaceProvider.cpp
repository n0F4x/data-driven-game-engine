#include "DefaultSurfaceProvider.hpp"

#include "plugins/Renderer.hpp"

namespace plugins::renderer {

[[nodiscard]]
auto DefaultSurfaceProvider::operator()(
    const App&                   app,
    const VkInstance             instance,
    const VkAllocationCallbacks* allocation_callbacks
) const -> std::optional<VkSurfaceKHR>
{
    return app.plugins().find<core::window::Window>().and_then(
        [instance, allocation_callbacks](const core::window::Window& window) {
            return window.create_vulkan_surface(instance, allocation_callbacks)
                .transform([](const VkSurfaceKHR surface) {
                    return std::make_optional(surface);
                })
                .value_or(std::nullopt);
        }
    );
}

auto DefaultSurfaceProvider::required_instance_settings_are_available(
    const vkb::SystemInfo& t_system_info
) -> bool
{
    return std::ranges::all_of(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::SystemInfo::is_extension_available, t_system_info)
    );
}

auto DefaultSurfaceProvider::enable_instance_settings(
    const vkb::SystemInfo&,
    vkb::InstanceBuilder& t_builder
) -> void
{
    std::ranges::for_each(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::InstanceBuilder::enable_extension, t_builder)
    );
}

auto DefaultSurfaceProvider::require_device_settings(vkb::PhysicalDeviceSelector&) -> void
{}

auto DefaultSurfaceProvider::enable_optional_device_settings(vkb::PhysicalDevice&) -> void
{}

}   // namespace plugins::renderer
