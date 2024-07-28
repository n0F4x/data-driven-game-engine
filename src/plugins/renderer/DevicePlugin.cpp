#include "DevicePlugin.hpp"

#include <spdlog/spdlog.h>

#include "app/App.hpp"
#include "core/config/vulkan.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"

static auto log_setup(const vkb::Device& t_device) -> void
{
    const uint32_t instance_version{ t_device.instance_version };

    SPDLOG_INFO(
        "Created Vulkan Instance with version: {}.{}.{}",
        VK_VERSION_MAJOR(instance_version),
        VK_VERSION_MINOR(instance_version),
        VK_VERSION_PATCH(instance_version)
    );

    const vk::PhysicalDeviceProperties properties{
        vk::PhysicalDevice(t_device.physical_device.physical_device).getProperties()
    };

    SPDLOG_INFO(
        "Chose GPU({}) with Vulkan version: {}.{}.{}",
        t_device.physical_device.name,
        VK_VERSION_MAJOR(properties.apiVersion),
        VK_VERSION_MINOR(properties.apiVersion),
        VK_VERSION_PATCH(properties.apiVersion)
    );

    std::string enabled_extensions{ "Enabled device extensions:" };
    for (const auto& extension : t_device.physical_device.get_extensions()) {
        enabled_extensions += '\n';
        enabled_extensions += '\t';
        enabled_extensions += extension;
    }
    SPDLOG_DEBUG(enabled_extensions);
}

namespace plugins::renderer {

auto DevicePlugin::operator()(
    App&                            app,
    const core::renderer::Instance& instance,
    const vk::UniqueSurfaceKHR&     surface
) const -> void
{
    vkb::PhysicalDeviceSelector physical_device_selector(
        static_cast<const vkb::Instance>(instance), surface.get()
    );

    // TODO: use std::bind_back
    for (const Dependency& dependency : m_dependencies) {
        if (dependency.require_device_settings) {
            dependency.require_device_settings(physical_device_selector);
        }
    }

    vkb::Result physical_device_result{ physical_device_selector.select() };
    if (!physical_device_result.has_value()) {
        SPDLOG_ERROR(physical_device_result.error().message());
        return;
    }
    vkb::PhysicalDevice& physical_device{ physical_device_result.value() };

    // TODO: use std::bind_back
    for (const Dependency& dependency : m_dependencies) {
        if (dependency.enable_optional_device_settings) {
            dependency.enable_optional_device_settings(physical_device);
        }
    }

    const vkb::DeviceBuilder device_builder{ physical_device };
    const auto               device_result{ device_builder.build() };
    if (!device_result.has_value()) {
        SPDLOG_ERROR(device_result.error().message());
        return;
    }

    const auto& device{ app.resources.emplace<core::renderer::Device>(device_result.value(
    )) };

    core::config::vulkan::init(device.get());

    log_setup(static_cast<const vkb::Device>(device));
}

}   // namespace plugins::renderer
