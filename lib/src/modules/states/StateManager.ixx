module;

#include <optional>
#include <utility>

export module ddge.modules.states.StateManager;

import ddge.modules.states.state_c;
import ddge.utility.containers.store.Store;

import ddge.utility.containers.OptionalRef;
import ddge.utility.meta.type_traits.const_like;
import ddge.utility.TypeList;

namespace ddge::states {

export class StateManager {
public:
    explicit StateManager(util::store::Store&& states);

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
    util::store::Store m_states;
};

}   // namespace ddge::states

template <ddge::states::state_c State_T, typename Self_T>
auto ddge::states::StateManager::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<std::optional<State_T>, Self_T>>
{
    return self.m_states.template find<std::optional<State_T>>();
}

template <ddge::states::state_c State_T, typename Self_T>
auto ddge::states::StateManager::at(this Self_T& self)
    -> util::meta::const_like_t<std::optional<State_T>, Self_T>&
{
    return self.m_states.template at<std::optional<State_T>>();
}

template <ddge::states::state_c State_T>
auto ddge::states::StateManager::contains() const noexcept -> bool
{
    return m_states.contains<std::optional<State_T>>();
}

module :private;

ddge::states::StateManager::StateManager(util::store::Store&& states)
    : m_states{ std::move(states) }
{}
