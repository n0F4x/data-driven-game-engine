module;

#include <functional>
#include <vector>

#include <VkBootstrap.h>

export module ddge.modules.renderer.InstanceInjection;

import ddge.modules.renderer.base.instance.Instance;

namespace ddge::renderer {

export class InstanceInjection {
public:
    struct Dependency {
        std::function<bool(const vkb::SystemInfo&)> required_settings_are_available;
        std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)> enable_settings;
    };

    static const Dependency debug_dependency;

    auto operator()() const -> base::Instance;

    template <typename Self>
    auto emplace_dependency(this Self&&, Dependency dependency) -> Self;

private:
    std::vector<Dependency> m_dependencies;
};

}   // namespace ddge::renderer

template <typename Self>
auto ddge::renderer::InstanceInjection::emplace_dependency(
    this Self&& self,
    Dependency  dependency
) -> Self
{
    self.m_dependencies.push_back(std::move(dependency));
    return std::forward<Self>(self);
}
