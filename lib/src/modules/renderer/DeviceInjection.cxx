module;

#include <format>

#include <VkBootstrap.h>

#include "modules/log/log_macros.hpp"

module ddge.modules.renderer.DeviceInjection;

import vulkan_hpp;

import ddge.modules.app.App;

import ddge.config.vulkan;

import ddge.modules.log;

import ddge.modules.renderer.base.instance.Instance;
import ddge.modules.renderer.base.device.Device;

static auto log_setup(const vkb::Device& device) -> void
{
    [[maybe_unused]]
    const uint32_t instance_version{ device.instance_version };

    ENGINE_LOG_INFO(
        std::format(
            "Created Vulkan Instance with version: {}.{}.{}",
            vk::apiVersionMajor(instance_version),
            vk::apiVersionMinor(instance_version),
            vk::apiVersionPatch(instance_version)
        )
    );

    [[maybe_unused]]
    const vk::PhysicalDeviceProperties properties{
        vk::PhysicalDevice(device.physical_device.physical_device).getProperties()
    };

    ENGINE_LOG_INFO(
        std::format(
            "Chose GPU({}) with Vulkan version: {}.{}.{}",
            device.physical_device.name,
            vk::apiVersionMajor(properties.apiVersion),
            vk::apiVersionMinor(properties.apiVersion),
            vk::apiVersionPatch(properties.apiVersion)
        )
    );

    std::string enabled_extensions{ "Enabled device extensions:" };
    for (const auto& extension : device.physical_device.get_extensions()) {
        enabled_extensions += '\n';
        enabled_extensions += '\t';
        enabled_extensions += extension;
    }
    ENGINE_LOG_DEBUG(enabled_extensions);
}

namespace ddge::renderer {

auto DeviceInjection::operator()(
    const base::Instance&       instance,
    const vk::UniqueSurfaceKHR& surface
) const -> base::Device
{
    vkb::PhysicalDeviceSelector physical_device_selector(
        static_cast<const vkb::Instance>(instance), surface.get()
    );

    for (const Dependency& dependency : m_dependencies) {
        if (dependency.require_settings) {
            dependency.require_settings(physical_device_selector);
        }
    }

    vkb::Result physical_device_result{ physical_device_selector.select() };
    if (!physical_device_result.has_value()) {
        ENGINE_LOG_ERROR(physical_device_result.error().message());
        throw std::runtime_error{ physical_device_result.error().message() };
    }
    vkb::PhysicalDevice& physical_device{ physical_device_result.value() };

    for (const Dependency& dependency : m_dependencies) {
        if (dependency.enable_optional_settings) {
            dependency.enable_optional_settings(physical_device);
        }
    }

    const vkb::DeviceBuilder device_builder{ physical_device };
    const auto               device_result{ device_builder.build() };
    if (!device_result.has_value()) {
        ENGINE_LOG_ERROR(device_result.error().message());
        throw std::runtime_error{ device_result.error().message() };
    }

    base::Device device{ device_result.value() };

    config::vulkan::init(device.get());

    log_setup(static_cast<const vkb::Device>(device));

    return device;
}

}   // namespace ddge::renderer
