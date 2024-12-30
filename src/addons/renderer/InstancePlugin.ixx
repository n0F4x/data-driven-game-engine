module;

#include <functional>
#include <vector>

#include <VkBootstrap.h>

export module addons.renderer.InstancePlugin;

import core.renderer.base.instance.Instance;

namespace addons::renderer {

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

}   // namespace addons::renderer

#include "InstancePlugin.inl"
