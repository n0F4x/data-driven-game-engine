#pragma once

#include <functional>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

namespace core::renderer::base {

class Instance;
class Device;

}   // namespace core::renderer::base

namespace plugins::renderer {

class DevicePlugin {
public:
    struct Dependency {
        std::function<void(vkb::PhysicalDeviceSelector&)> require_device_settings;
        std::function<void(vkb::PhysicalDevice&)>         enable_optional_device_settings;
    };

    auto operator()(
        const core::renderer::base::Instance& instance,
        const vk::UniqueSurfaceKHR&           surface
    ) const -> core::renderer::base::Device;

    template <typename... Args>
    auto emplace_dependency(Args&&... args) -> DevicePlugin&;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace plugins::renderer

#include "DevicePlugin.inl"
