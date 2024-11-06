#include "DevicePlugin.hpp"

#include <spdlog/spdlog.h>

#include "app/App.hpp"
#include "core/config/vulkan.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"

static auto log_setup(const vkb::Device& device) -> void
{
    const uint32_t instance_version{ device.instance_version };

    SPDLOG_INFO(
        "Created Vulkan Instance with version: {}.{}.{}",
        vk::apiVersionMajor(instance_version),
        vk::apiVersionMinor(instance_version),
        vk::apiVersionPatch(instance_version)
    );

    const vk::PhysicalDeviceProperties properties{
        vk::PhysicalDevice(device.physical_device.physical_device).getProperties()
    };

    SPDLOG_INFO(
        "Chose GPU({}) with Vulkan version: {}.{}.{}",
        device.physical_device.name,
        vk::apiVersionMajor(properties.apiVersion),
        vk::apiVersionMinor(properties.apiVersion),
        vk::apiVersionPatch(properties.apiVersion)
    );

    std::string enabled_extensions{ "Enabled device extensions:" };
    for (const auto& extension : device.physical_device.get_extensions()) {
        enabled_extensions += '\n';
        enabled_extensions += '\t';
        enabled_extensions += extension;
    }
    SPDLOG_DEBUG(enabled_extensions);
}

namespace plugins::renderer {

auto DevicePlugin::operator()(
    const core::renderer::base::Instance& instance,
    const vk::UniqueSurfaceKHR&           surface
) const -> core::renderer::base::Device
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
        throw std::runtime_error{ physical_device_result.error().message() };
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
        throw std::runtime_error{ device_result.error().message() };
    }

    core::renderer::base::Device device{ device_result.value() };

    core::config::vulkan::init(device.get());

    log_setup(static_cast<const vkb::Device>(device));

    return device;
}

}   // namespace plugins::renderer
