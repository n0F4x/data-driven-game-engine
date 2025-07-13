module;

#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

export module core.states.StateManager;

import core.states.state_c;

import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace core::states {

export template <state_c... States_T>
class StateManager {
public:
    template <state_c State_T>
    constexpr static std::bool_constant<
        util::meta::type_list_contains_v<util::TypeList<States_T...>, State_T>>
        registered;

    template <state_c State_T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&)
        -> util::meta::forward_like_t<std::optional<State_T>, Self_T>
        requires(registered<State_T>());

private:
    std::tuple<std::optional<States_T>...> m_states;
};

}   // namespace core::states

template <core::states::state_c... States_T>
template <core::states::state_c State_T, typename Self_T>
constexpr auto core::states::StateManager<States_T...>::get(this Self_T&& self)
    -> util::meta::forward_like_t<std::optional<State_T>, Self_T>
    requires(registered<State_T>())
{
    return std::get<std::optional<State_T>>(std::forward_like<Self_T>(self.m_states));
}
