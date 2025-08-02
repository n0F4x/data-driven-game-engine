module;


#include <VkBootstrap.h>

module core.renderer.base.swapchain.Swapchain;

namespace core::renderer::base {

auto Swapchain::Requirements::
    required_instance_settings_are_available(const vkb::SystemInfo&) -> bool
{
    return true;
}

auto Swapchain::Requirements::
    enable_instance_settings(const vkb::SystemInfo&, vkb::InstanceBuilder&) -> void
{}

auto Swapchain::Requirements::require_device_settings(
    vkb::PhysicalDeviceSelector& physical_device_selector
) -> void
{
    physical_device_selector.add_required_extension(vk::KHRSwapchainExtensionName);
}

auto Swapchain::Requirements::enable_optional_device_settings(vkb::PhysicalDevice&)
    -> void
{}

}   // namespace core::renderer::base
