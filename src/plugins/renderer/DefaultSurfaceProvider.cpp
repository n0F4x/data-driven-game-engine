#include "DefaultSurfaceProvider.hpp"

#include "plugins/Renderer.hpp"

namespace plugins::renderer {

DefaultSurfaceProvider::DefaultSurfaceProvider(const window::Window& window) noexcept
    : m_window(window)
{}

[[nodiscard]]
auto DefaultSurfaceProvider::operator()(
    const VkInstance             t_instance,
    const VkAllocationCallbacks* t_allocation_callbacks
) const -> std::expected<VkSurfaceKHR, VkResult>
{
    return m_window.get().create_vulkan_surface(t_instance, t_allocation_callbacks);
}

auto DefaultSurfaceProvider::required_instance_settings_are_available(
    const vkb::SystemInfo& t_system_info
) -> bool
{
    return std::ranges::all_of(
        core::window::Window::vulkan_instance_extensions(),
        [&t_system_info](const auto* const extension_name) {
            return t_system_info.is_extension_available(extension_name);
        }
    );
}

auto DefaultSurfaceProvider::enable_instance_settings(
    const vkb::SystemInfo&,
    vkb::InstanceBuilder& t_builder
) -> void
{
    for (const auto* const extension_name :
         core::window::Window::vulkan_instance_extensions())
    {
        t_builder.enable_extension(extension_name);
    }
}

auto DefaultSurfaceProvider::require_device_settings(vkb::PhysicalDeviceSelector&) -> void
{}

auto DefaultSurfaceProvider::enable_optional_device_settings(vkb::PhysicalDevice&) -> void
{}

}   // namespace plugins::renderer
