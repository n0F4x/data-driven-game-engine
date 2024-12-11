module;

#include <algorithm>
#include <expected>

#include <spdlog/spdlog.h>

#include <VkBootstrap.h>

module plugins.renderer.SurfacePlugin;

import core.app.App;
import core.store.StoreView;
import core.renderer.base.instance.Instance;
import core.window.Window;
import plugins.renderer.InstancePlugin;

auto plugins::renderer::SurfacePlugin::operator()(
    const core::window::Window&           window,
    const core::renderer::base::Instance& instance
) const -> vk::UniqueSurfaceKHR
{
    const std::expected<VkSurfaceKHR, VkResult> expected_surface{
        window.create_vulkan_surface(instance.get(), nullptr)
    };

    if (!expected_surface.has_value()) {
        SPDLOG_ERROR("Vulkan surface creation failed");
        throw std::runtime_error{ "Vulkan surface creation failed" };
    }

    return vk::UniqueSurfaceKHR{ expected_surface.value(), instance.get() };
}

static auto required_instance_settings_are_available(const vkb::SystemInfo& system_info)
    -> bool
{
    return std::ranges::all_of(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::SystemInfo::is_extension_available, system_info)
    );
}

static auto enable_instance_settings(const vkb::SystemInfo&, vkb::InstanceBuilder& builder)
    -> void
{
    std::ranges::for_each(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::InstanceBuilder::enable_extension, builder)
    );
}

auto plugins::renderer::SurfacePlugin::setup(const StoreView plugins) -> void
{
    const std::optional optional_instance_plugin{ plugins.find<InstancePlugin>() };
    if (!optional_instance_plugin.has_value()) {
        throw std::runtime_error{
            "plugins::renderer::base::InstancePlugin required but not found in 'plugins'"
        };
    }
    InstancePlugin& instance_plugin{ optional_instance_plugin.value().get() };

    instance_plugin.emplace_dependency(
        InstancePlugin::Dependency{
            .required_settings_are_available = ::required_instance_settings_are_available,
            .enable_settings                 = ::enable_instance_settings,
        }
    );
}
