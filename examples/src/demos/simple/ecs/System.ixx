module;

#include <functional>
#include <utility>

#include <entt/entity/registry.hpp>

export module ecs:System;

import :Query;
import :Registry;
import :Res;
import :schedulable_c;

import core.store.Store;

import utility.tuple;
import utility.meta.type_traits.is_specialization_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.type_list.type_list_filter;

namespace ecs {

export class System {
public:
    template <schedulable_c Task_T>
    System(Task_T&& task, core::store::Store& store, Registry& registry);

    auto operator()() const -> void;

private:
    std::function<void()> m_invocation;
};

}   // namespace ecs

template <typename System_T>
auto collect_params_of(core::store::Store& store, ecs::Registry& registry)
{
    using ParamsTuple =
        util::meta::arguments_of_t<std::remove_pointer_t<std::decay_t<System_T>>>;

    return util::generate_tuple_from<ParamsTuple>([&store, &registry]<typename Param_T>() {
        if constexpr (util::meta::is_specialization_of_v<Param_T, ecs::Res>) {
            return std::function<Param_T()>{
                [res =
                     Param_T{ store.at<std::remove_cvref_t<typename Param_T::Underlying>>(
                     ) }] { return res; }
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

template <ecs::schedulable_c Task_T>
ecs::System::System(Task_T&& task, core::store::Store& store, Registry& registry)
    : m_invocation{ [param_builders_tuple = ::collect_params_of<Task_T>(store, registry),
                     task                 = std::forward<Task_T>(task)] {
          std::apply(
              task,
              std::apply(
                  []<typename... ParamBuilders_T>(ParamBuilders_T&&... param_builders)
                      -> util::meta::arguments_of_t<
                          std::remove_pointer_t<std::decay_t<Task_T>>> {
                      return std::make_tuple(
                          std::invoke(std::forward<ParamBuilders_T>(param_builders))...
                      );
                  },
                  param_builders_tuple
              )
          );
      } }
{}
