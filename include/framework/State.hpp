#pragma once

#include <functional>
#include <utility>

#include <gsl/pointers>

#include "common/patterns/builder/helper.hpp"

namespace fw {

template <typename Id>
class StateBase final {
    ///----------------///
    ///  Member types  ///
    ///----------------///

public:
    using Action = std::function<void()>;

private:
    class Builder;
    friend BuilderBase<StateBase>;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] StateBase(const StateBase&) = delete;
    [[nodiscard]] StateBase(StateBase&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto id() const noexcept -> Id;
    [[nodiscard]] auto invalid() const noexcept -> bool;
    void entered() const noexcept;
    void exited() const noexcept;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    template <Id t_id>
        requires(t_id != 0)
    [[nodiscard]] static auto create() noexcept -> Builder;
    [[nodiscard]] static auto invalid_state() noexcept
        -> gsl::not_null<const StateBase<Id>*>;

private:
    [[nodiscard]] explicit StateBase(Id t_id = {}) noexcept;

    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    static const StateBase s_invalidStateBase;

    Id m_id{};
    Action m_enterAction;
    Action m_exitAction;
};

template <typename Id>
class StateBase<Id>::Builder final : public BuilderBase<StateBase<Id>> {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    using BuilderBase<StateBase<Id>>::BuilderBase;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto on_enter(Action&& t_callback) noexcept -> Builder&;
    [[nodiscard]] auto on_exit(Action&& t_callback) noexcept -> Builder&;
};

template <typename Id>
inline const StateBase<Id> StateBase<Id>::s_invalidStateBase;

template <typename Id>
gsl::not_null<const StateBase<Id>*> StateBase<Id>::invalid_state() noexcept {
    return &s_invalidStateBase;
}

template <typename Id>
auto StateBase<Id>::id() const noexcept -> Id {
    return m_id;
}

template <typename Id>
auto StateBase<Id>::invalid() const noexcept -> bool {
    return m_id == 0;
}

template <typename Id>
void StateBase<Id>::entered() const noexcept {
    if (m_enterAction) {
        m_enterAction();
    }
}

template <typename Id>
void StateBase<Id>::exited() const noexcept {
    if (m_exitAction) {
        m_exitAction();
    }
}

template <typename Id>
template <Id t_id>
    requires(t_id != 0)
auto StateBase<Id>::create() noexcept -> StateBase<Id>::Builder {
    return Builder{ t_id };
}

template <typename Id>
StateBase<Id>::StateBase(Id t_id) noexcept : m_id{ t_id } {}

template <typename Id>
auto StateBase<Id>::Builder::on_enter(Action&& t_callback) noexcept
    -> Builder& {
    BuilderBase<StateBase<Id>>::draft().m_enterAction = std::move(t_callback);

    return *this;
}

template <typename Id>
auto StateBase<Id>::Builder::on_exit(Action&& t_callback) noexcept -> Builder& {
    BuilderBase<StateBase<Id>>::draft().m_exitAction = std::move(t_callback);

    return *this;
}

using State = StateBase<uint32_t>;

}   // namespace fw
