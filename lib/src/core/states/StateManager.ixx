module;

#include <optional>
#include <utility>

export module core.states.StateManager;

import core.states.state_c;
import core.store.Store;

import utility.containers.OptionalRef;
import utility.meta.type_traits.const_like;
import utility.TypeList;

namespace core::states {

export class StateManager {
public:
    template <state_c... States_T>
    StateManager(util::TypeList<States_T...>);

    template <state_c State_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::meta::const_like_t<std::optional<State_T>, Self_T>>;

    template <state_c State_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&) -> util::meta::const_like_t<std::optional<State_T>, Self_T>&;

    template <state_c State_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    store::Store m_states;
};

}   // namespace core::states

template <core::states::state_c... States_T>
core::states::StateManager::StateManager(util::TypeList<States_T...>)
{
    (m_states.emplace<std::optional<States_T>>(), ...);
}

template <core::states::state_c State_T, typename Self_T>
auto core::states::StateManager::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<std::optional<State_T>, Self_T>>
{
    return self.m_states.template find<std::optional<State_T>>();
}

template <core::states::state_c State_T, typename Self_T>
auto core::states::StateManager::at(this Self_T& self)
    -> util::meta::const_like_t<std::optional<State_T>, Self_T>&
{
    return self.m_states.template at<std::optional<State_T>>();
}

template <core::states::state_c State_T>
auto core::states::StateManager::contains() const noexcept -> bool
{
    return m_states.contains<std::optional<State_T>>();
}
