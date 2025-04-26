module;

#include <functional>
#include <utility>

export module extensions.scheduler.accessors.ecs.Query;

import core.ecs;

namespace extensions::scheduler::accessors::ecs {

export template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
class Query {
public:
    explicit Query(core::ecs::Registry& registry);

    template <typename F>
    auto for_each(F&& func) const -> F;

private:
    std::reference_wrapper<core::ecs::Registry> m_registry;
};

}   // namespace extensions::scheduler::accessors::ecs

template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
extensions::scheduler::accessors::ecs::Query<Parameters_T...>::Query(
    core::ecs::Registry& registry
)
    : m_registry{ registry }
{}

template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
template <typename F>
auto extensions::scheduler::accessors::ecs::Query<Parameters_T...>::for_each(
    F&& func
) const -> F
{
    return core::ecs::query<Parameters_T...>(m_registry, std::forward<F>(func));
}
