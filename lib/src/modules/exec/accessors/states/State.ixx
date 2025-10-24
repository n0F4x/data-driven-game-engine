module;

#include <functional>
#include <optional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.accessors.states:State;

import ddge.modules.states.state_c;
import ddge.modules.exec.locks.Lockable;
import ddge.modules.exec.locks.ReaderLock;
import ddge.modules.exec.locks.WriterLock;

import ddge.utility.meta.type_traits.const_like;
import ddge.utility.contracts;

namespace ddge::exec::accessors {

inline namespace states {

template <typename State_T>
struct StateLock : std::conditional_t<
                       std::is_const_v<State_T>,
                       ReaderLock<StateLock<std::remove_const_t<State_T>>>,
                       WriterLock<StateLock<std::remove_const_t<State_T>>>> {};

export template <typename State_T>
    requires ddge::states::state_c<std::remove_const_t<State_T>>
class State : public Lockable<StateLock<State_T>> {
public:
    using Underlying = State_T;

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

}   // namespace states

}   // namespace ddge::exec::accessors

template <typename State_T>
    requires ddge::states::state_c<std::remove_const_t<State_T>>
constexpr ddge::exec::accessors::states::State<State_T>::State(
    util::meta::const_like_t<std::optional<std::remove_const_t<State_T>>, State_T>& state
)
    : m_state_ref{ state }
{}

template <typename State_T>
    requires ddge::states::state_c<std::remove_const_t<State_T>>
constexpr auto ddge::exec::accessors::states::State<State_T>::operator->() const
    -> State_T*
{
    PRECOND(has_value());

    return m_state_ref.get().operator->();
}

template <typename State_T>
    requires ddge::states::state_c<std::remove_const_t<State_T>>
constexpr auto ddge::exec::accessors::states::State<State_T>::operator*() const
    -> State_T&
{
    PRECOND(has_value());

    return m_state_ref.get().operator*();
}

template <typename State_T>
    requires ddge::states::state_c<std::remove_const_t<State_T>>
constexpr auto ddge::exec::accessors::states::State<State_T>::has_value() const -> bool
{
    return m_state_ref.get().has_value();
}

template <typename State_T>
    requires ddge::states::state_c<std::remove_const_t<State_T>>
template <typename... Args_T>
constexpr auto ddge::exec::accessors::states::State<State_T>::emplace(
    Args_T&&... args
) const -> State_T& requires(!std::is_const_v<State_T>) {
    return m_state_ref.get().emplace(std::forward<Args_T>(args)...);
}

template <typename State_T>
    requires ddge::states::state_c<std::remove_const_t<State_T>>
constexpr auto ddge::exec::accessors::states::State<State_T>::reset() const -> void
    requires(!std::is_const_v<State_T>)
{
    m_state_ref.get().reset();
}
