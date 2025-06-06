module;

#include <functional>
#include <vector>

#include <VkBootstrap.h>

export module plugins.renderer.DevicePlugin;

import vulkan_hpp;

import core.renderer.base.instance.Instance;
import core.renderer.base.device.Device;

namespace plugins::renderer {

export class DevicePlugin {
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

template <typename Self>
auto plugins::renderer::DevicePlugin::emplace_dependency(
    this Self&& self,
    Dependency  dependency
) -> Self
{
    self.m_dependencies.push_back(dependency);
    return std::forward<Self>(self);
}
