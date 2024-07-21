#include "DevicePlugin.hpp"

#include "app/App.hpp"
#include "core/config/vulkan.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"

namespace plugins::renderer {

auto DevicePlugin::operator()(App& app) const -> void
{
    const std::optional instance_result{ app.resources.find<core::renderer::Instance>() };
    if (!instance_result.has_value()) {
        throw std::runtime_error{
            "core::renderer::Instance required but not found in 'app.resources'"
        };
    }
    const core::renderer::Instance& instance{ instance_result.value().get() };

    const std::optional surface_result{ app.resources.find<vk::UniqueSurfaceKHR>() };
    if (!surface_result.has_value()) {
        throw std::runtime_error{
            "vk::UniqueSurfaceKHR required but not found in 'app.resources'"
        };
    }
    const vk::UniqueSurfaceKHR& surface{ surface_result.value().get() };


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
}

}   // namespace plugins::renderer
