module;

#include <functional>
#include <vector>

#include <VkBootstrap.h>

export module plugins.renderer.InstancePlugin;

import core.renderer.base.instance.Instance;

namespace plugins::renderer {

export class InstancePlugin {
public:
    struct Dependency {
        std::function<bool(const vkb::SystemInfo&)> required_settings_are_available;
        std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)> enable_settings;
    };

    InstancePlugin();

    auto operator()() const -> core::renderer::base::Instance;

    template <typename Self>
    auto emplace_dependency(this Self&&, Dependency dependency) -> Self;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace plugins::renderer

#include "InstancePlugin.inl"
