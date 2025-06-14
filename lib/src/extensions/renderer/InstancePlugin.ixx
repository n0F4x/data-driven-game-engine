module;

#include <functional>
#include <vector>

#include <VkBootstrap.h>

export module extensions.renderer.InstancePlugin;

import core.renderer.base.instance.Instance;

namespace extensions::renderer {

export class InstancePlugin {
public:
    struct Dependency {
        std::function<bool(const vkb::SystemInfo&)> required_settings_are_available;
        std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)> enable_settings;
    };

    static const Dependency debug_dependency;

    auto operator()() const -> core::renderer::base::Instance;

    template <typename Self>
    auto emplace_dependency(this Self&&, Dependency dependency) -> Self;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace extensions::renderer

template <typename Self>
auto extensions::renderer::InstancePlugin::emplace_dependency(
    this Self&& self,
    Dependency  dependency
) -> Self
{
    self.m_dependencies.push_back(std::move(dependency));
    return std::forward<Self>(self);
}
