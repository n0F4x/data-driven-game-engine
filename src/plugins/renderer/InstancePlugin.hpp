#pragma once

#include <functional>
#include <vector>

#include <VkBootstrap.h>

namespace core::renderer::base {

class Instance;

}   // namespace core::renderer::base

namespace plugins::renderer {

class InstancePlugin {
public:
    struct Dependency {
        std::function<bool(const vkb::SystemInfo&)>
            required_instance_settings_are_available;
        std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)>
            enable_instance_settings;
    };

    InstancePlugin();

    auto operator()() const -> core::renderer::base::Instance;

    template <typename... Args>
    auto emplace_dependency(Args&&... args) -> InstancePlugin&;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace plugins::renderer

#include "InstancePlugin.inl"
