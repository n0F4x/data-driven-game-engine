module;

#include <entt/entity/registry.hpp>

export module ecs:Registry;

namespace ecs {

export class Registry {
public:
    template <typename Self_T>
    [[nodiscard]]
    auto&& get(this Self_T&&);

private:
    entt::registry m_registry;
};

template <typename Self_T>
auto&& Registry::get(this Self_T&& self)
{
    return std::forward_like<Self_T>(self.m_registry);
}

}   // namespace ecs
