module;

#include <algorithm>
#include <expected>
#include <functional>

#include <VkBootstrap.h>

#include "modules/log/log_macros.hpp"

module ddge.modules.renderer.SurfaceInjection;

import ddge.modules.app.App;
import ddge.modules.log;
import ddge.modules.renderer.base.instance.Instance;
import ddge.modules.window.Window;
import ddge.modules.renderer.InstanceInjection;

auto ddge::renderer::SurfaceInjection::operator()(
    const window::Window& window,
    const base::Instance& instance
) const -> vk::UniqueSurfaceKHR
{
    const std::expected<VkSurfaceKHR, VkResult> expected_surface{
        window.create_vulkan_surface(instance.get(), nullptr)
    };

    if (!expected_surface.has_value()) {
        ENGINE_LOG_ERROR("Vulkan surface creation failed");
        throw std::runtime_error{ "Vulkan surface creation failed" };
    }

    std::bind_front(&SurfaceInjection::setup, *this);
    return vk::UniqueSurfaceKHR{ expected_surface.value(), instance.get() };
}

static auto required_instance_settings_are_available(const vkb::SystemInfo& system_info)
    -> bool
{
    return std::ranges::all_of(
        ddge::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::SystemInfo::is_extension_available, system_info)
    );
}

static auto enable_instance_settings(const vkb::SystemInfo&, vkb::InstanceBuilder& builder)
    -> void
{
    std::ranges::for_each(
        ddge::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::InstanceBuilder::enable_extension, builder)
    );
}

auto ddge::renderer::SurfaceInjection::setup(InstanceInjection& instance_injection)
    -> void
{
    instance_injection.emplace_dependency(
        InstanceInjection::Dependency{
            .required_settings_are_available = ::required_instance_settings_are_available,
            .enable_settings                 = ::enable_instance_settings,
        }
    );
}
