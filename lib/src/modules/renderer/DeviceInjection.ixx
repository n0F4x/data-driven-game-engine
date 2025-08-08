module;

#include <functional>
#include <vector>

#include <VkBootstrap.h>

export module modules.renderer.DeviceInjection;

import vulkan_hpp;

import modules.renderer.base.instance.Instance;
import modules.renderer.base.device.Device;

namespace modules::renderer {

export class DeviceInjection {
public:
    struct Dependency {
        std::function<void(vkb::PhysicalDeviceSelector&)> require_settings;
        std::function<void(vkb::PhysicalDevice&)>         enable_optional_settings;
    };

    auto operator()(
        const base::Instance& instance,
        const vk::UniqueSurfaceKHR&           surface
    ) const -> base::Device;

    template <typename Self>
    auto emplace_dependency(this Self&&, Dependency dependency) -> Self;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace modules::renderer

template <typename Self>
auto modules::renderer::DeviceInjection::emplace_dependency(
    this Self&& self,
    Dependency  dependency
) -> Self
{
    self.m_dependencies.push_back(dependency);
    return std::forward<Self>(self);
}
