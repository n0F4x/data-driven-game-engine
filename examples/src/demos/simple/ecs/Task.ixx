module;

#include <functional>
#include <utility>

#include <entt/entity/registry.hpp>

export module ecs:Task;

import :Query;
import :Registry;
import :Res;
import :system_c;

import core.store.Store;

import utility.tuple;
import utility.meta.type_traits.is_specialization_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_filter;

namespace ecs {

export class Task {
public:
    template <system_c System_T, typename... Resources_T>
    Task(System_T&& system, std::tuple<Resources_T...>& resources, Registry& registry);

    auto operator()() const -> void;

private:
    std::function<void()> m_invocation;
};

}   // namespace ecs

template <typename System_T, typename... Resources_T>
auto collect_params_of(std::tuple<Resources_T...>& resources, ecs::Registry& registry)
{
    using ParamsTuple =
        util::meta::arguments_of_t<std::remove_pointer_t<std::decay_t<System_T>>>;

    return util::generate_tuple_from<ParamsTuple>([&resources,
                                                   &registry]<typename Param_T>() {
        if constexpr (util::meta::is_specialization_of_v<Param_T, ecs::Res>) {
            static_assert(
                util::meta::type_list_contains_v<
                    std::tuple<Resources_T...>,
                    typename Param_T::Underlying>,
                // TODO (reflection): Specify resource name in assertion string
                "The required resource is missing."
            );
            return [res = Param_T{ std::get<typename Param_T::Underlying>(resources) }] {
                return res;
            };
        }
        else if constexpr (util::meta::is_specialization_of_v<Param_T, ecs::Query>) {
            return [&registry] { return Param_T{ registry.get() }; };
        }
        else {
            static_assert(false, "invalid system parameter");
            std::unreachable();
        }
    });
}

template <ecs::system_c System_T, typename... Resources_T>
ecs::Task::Task(
    System_T&&                  system,
    std::tuple<Resources_T...>& resources,
    Registry&                   registry
)
    : m_invocation{ [param_builders_tuple =
                         ::collect_params_of<System_T>(resources, registry),
                     system = std::forward<System_T>(system)] {
          std::apply(
              system,
              std::apply(
                  []<typename... ParamBuilders_T>(ParamBuilders_T&&... param_builders)
                      -> util::meta::arguments_of_t<
                          std::remove_pointer_t<std::decay_t<System_T>>> {
                      return std::make_tuple(
                          std::invoke(std::forward<ParamBuilders_T>(param_builders))...
                      );
                  },
                  param_builders_tuple
              )
          );
      } }
{}
