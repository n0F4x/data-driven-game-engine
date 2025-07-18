module;

#include <functional>
#include <optional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module extensions.scheduler.accessors.states.State;

import core.states.state_c;

import utility.meta.type_traits.const_like;
import utility.contracts;

namespace extensions::scheduler::accessors::states {

export template <typename State_T>
    requires core::states::state_c<std::remove_const_t<State_T>>
class State {
public:
    constexpr explicit State(
        util::meta::const_like_t<std::optional<std::remove_const_t<State_T>>, State_T>& state
    );

    constexpr auto operator->() const -> State_T*;
    constexpr auto operator*() const -> State_T&;

    [[nodiscard]]
    constexpr auto has_value() const -> bool;

    template <typename... Args_T>
    constexpr auto emplace(Args_T&&... args) const
        -> State_T& requires(!std::is_const_v<State_T>);

    constexpr auto reset() const -> void
        requires(!std::is_const_v<State_T>);

private:
    std::reference_wrapper<
        util::meta::const_like_t<std::optional<std::remove_const_t<State_T>>, State_T>>
        m_state_ref;
};

}   // namespace extensions::scheduler::accessors::states

template <typename State_T>
    requires core::states::state_c<std::remove_const_t<State_T>>
constexpr extensions::scheduler::accessors::states::State<State_T>::State(
    util::meta::const_like_t<std::optional<std::remove_const_t<State_T>>, State_T>& state
)
    : m_state_ref{ state }
{}

template <typename State_T>
    requires core::states::state_c<std::remove_const_t<State_T>>
constexpr auto extensions::scheduler::accessors::states::State<State_T>::operator->() const
    -> State_T*
{
    PRECOND(has_value());

    return m_state_ref.get().operator->();
}

template <typename State_T>
    requires core::states::state_c<std::remove_const_t<State_T>>
constexpr auto extensions::scheduler::accessors::states::State<State_T>::operator*() const
    -> State_T&
{
    PRECOND(has_value());

    return m_state_ref.get().operator*();
}

template <typename State_T>
    requires core::states::state_c<std::remove_const_t<State_T>>
constexpr auto extensions::scheduler::accessors::states::State<State_T>::has_value() const
    -> bool
{
    return m_state_ref.get().has_value();
}

template <typename State_T>
    requires core::states::state_c<std::remove_const_t<State_T>>
template <typename... Args_T>
constexpr auto extensions::scheduler::accessors::states::State<State_T>::emplace(
    Args_T&&... args
) const -> State_T& requires(!std::is_const_v<State_T>) {
    return m_state_ref.get().emplace(std::forward<Args_T>(args)...);
}

template <typename State_T>
    requires core::states::state_c<std::remove_const_t<State_T>>
constexpr auto extensions::scheduler::accessors::states::State<State_T>::reset() const
    -> void
    requires(!std::is_const_v<State_T>)
{
    m_state_ref.get().reset();
}
