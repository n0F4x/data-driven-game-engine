#pragma once

#include <expected>
#include <optional>

#include "engine/app/config.hpp"
#include "vulkan/helpers.hpp"

#include "Device.hpp"
#include "Renderer.hpp"

namespace engine::renderer {

namespace vulkan::defaults {

template <typename T>
auto create() noexcept -> std::expected<T, vk::Result>
{}

template <>
auto create<vk::Instance>() noexcept -> std::expected<vk::Instance, vk::Result>
{
    vk::ApplicationInfo app_info{ .apiVersion = VK_API_VERSION_1_3 };
    auto                layers{ renderer::vulkan::validation_layers() };
    auto                extensions{ renderer::vulkan::instance_extensions() };

    vk::InstanceCreateInfo create_info{
        .pApplicationInfo        = &app_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    auto [result, instance]{ vk::createInstance(create_info) };
    if (result == vk::Result::eSuccess) {
        return instance;
    }
    return std::unexpected{ result };
}

}   // namespace vulkan::defaults

namespace defaults {

template <typename T>
auto create(vk::Instance, vk::SurfaceKHR, vk::PhysicalDevice) noexcept
    -> std::optional<T>
{}

const std::vector<const char*> g_device_extensions{
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
};

template <>
auto create<Device>(
    vk::Instance       t_instance,
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device
) noexcept -> std::optional<Device>
{
    vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{
        .dynamicRendering = true
    };

    return Device::create(
        t_instance,
        t_surface,
        t_physical_device,
        Device::Config{ .extensions = g_device_extensions,
                        .next       = &dynamic_rendering_features }
    );
}

template <typename T>
auto create(app::config::Window&) noexcept -> std::optional<T>
{}

template <>
auto create<Renderer>(app::config::Window& t_window) noexcept
    -> std::optional<Renderer>
{
    auto instance{ vulkan::defaults::create<vk::Instance>() };
    if (!instance.has_value()) {
        return std::nullopt;
    }

    Renderer::Config config{
        .create_debug_messenger = vulkan::create_debug_messenger,
        .filter_physical_device =
            [](vk::PhysicalDevice t_physical_device, vk::SurfaceKHR) {
                return vulkan::supports_extensions(
                    t_physical_device, g_device_extensions
                );
            }
    };
    auto surface{ t_window.create_vulkan_surface(*instance, nullptr) };
    if (!surface.has_value()) {
        return std::nullopt;
    }
    auto framebuffer_size{ [&]() {
        auto framebuffer_size{ t_window.framebuffer_size() };
        return vk::Extent2D{ .width  = framebuffer_size.x,
                             .height = framebuffer_size.y };
    }() };
    auto create_device{ [](vk::Instance       t_instance,
                           vk::SurfaceKHR     t_surface,
                           vk::PhysicalDevice t_physical_device) noexcept {
        return create<Device>(t_instance, t_surface, t_physical_device);
    } };

    return Renderer::create(
        *instance, *surface, framebuffer_size, create_device, config
    );
}

}   // namespace defaults

}   // namespace engine::renderer
