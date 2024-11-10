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
        std::function<void(vkb::PhysicalDeviceSelector&)> require_settings;
        std::function<void(vkb::PhysicalDevice&)>         enable_optional_settings;
    };

    auto operator()(
        const core::renderer::base::Instance& instance,
        const vk::UniqueSurfaceKHR&           surface
    ) const -> core::renderer::base::Device;

    template <typename Self>
    auto emplace_dependency(this Self&&, Dependency dependency) -> Self;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace plugins::renderer

#include "DevicePlugin.inl"
