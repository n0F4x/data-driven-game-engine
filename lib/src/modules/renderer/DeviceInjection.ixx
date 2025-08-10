module;

#include <functional>
#include <vector>

#include <VkBootstrap.h>

export module ddge.modules.renderer.DeviceInjection;

import vulkan_hpp;

import ddge.modules.renderer.base.instance.Instance;
import ddge.modules.renderer.base.device.Device;

namespace ddge::renderer {

export class DeviceInjection {
public:
    struct Dependency {
        std::function<void(vkb::PhysicalDeviceSelector&)> require_settings;
        std::function<void(vkb::PhysicalDevice&)>         enable_optional_settings;
    };

    auto operator()(
        const base::Instance&       instance,
        const vk::UniqueSurfaceKHR& surface
    ) const -> base::Device;

    template <typename Self>
    auto emplace_dependency(this Self&&, Dependency dependency) -> Self;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace ddge::renderer

template <typename Self>
auto ddge::renderer::DeviceInjection::emplace_dependency(
    this Self&& self,
    Dependency  dependency
) -> Self
{
    self.m_dependencies.push_back(dependency);
    return std::forward<Self>(self);
}
