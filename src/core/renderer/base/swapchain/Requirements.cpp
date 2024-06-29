#include "Requirements.hpp"

namespace core::renderer {

auto Swapchain::Requirements::
    required_instance_settings_are_available(const vkb::SystemInfo&) -> bool
{
    return true;
}

auto Swapchain::Requirements::
    enable_instance_settings(const vkb::SystemInfo&, vkb::InstanceBuilder&) -> void
{}

auto Swapchain::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& t_physical_device_selector
) -> void
{
    t_physical_device_selector.add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

auto Swapchain::Requirements::enable_optional_device_settings(vkb::PhysicalDevice&) -> void
{}

}   // namespace core::renderer
