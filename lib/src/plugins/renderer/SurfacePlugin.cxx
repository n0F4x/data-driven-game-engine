module;

#include <algorithm>
#include <expected>
#include <functional>

#include <VkBootstrap.h>

#include "core/log/log_macros.hpp"

module plugins.renderer.SurfacePlugin;

import core.app.App;
import core.log;
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
        ENGINE_LOG_ERROR("Vulkan surface creation failed");
        throw std::runtime_error{ "Vulkan surface creation failed" };
    }

    std::bind_front(&SurfacePlugin::setup, *this);
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

auto plugins::renderer::SurfacePlugin::setup(InstancePlugin& instance_plugin) -> void
{
    instance_plugin.emplace_dependency(
        InstancePlugin::Dependency{
            .required_settings_are_available = ::required_instance_settings_are_available,
            .enable_settings                 = ::enable_instance_settings,
        }
    );
}
