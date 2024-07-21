#pragma once

#include <functional>
#include <vector>

#include <VkBootstrap.h>

class App;

namespace plugins::renderer {

class DevicePlugin {
public:
    struct Dependency {
        std::function<void(vkb::PhysicalDeviceSelector&)> require_device_settings;
        std::function<void(vkb::PhysicalDevice&)>         enable_optional_device_settings;
    };

    auto operator()(App& app) const -> void;

    template <typename... Args>
    auto emplace_dependency(Args&&... args) -> DevicePlugin&;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace plugins::renderer

#include "DevicePlugin.inl"
