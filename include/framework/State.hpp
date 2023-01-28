#pragma once

#include <functional>
#include <utility>

#include <gsl/pointers>

namespace fw {

template <typename Id>
class StateBase final {
public:
    ///----------------///
    ///  Member types  ///
    ///----------------///
    using IdType = Id;
    using Action = std::function<void()>;

private:
    class Builder;
    friend Builder;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] StateBase(const StateBase&) noexcept = default;
    [[nodiscard]] StateBase(StateBase&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto id() const noexcept -> IdType;
    void enter() const noexcept;
    void exit() const noexcept;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    [[nodiscard]] explicit StateBase(IdType t_id = {},
                                     Action&& t_enterAction = {},
                                     Action&& t_exitAction = {}) noexcept;

    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    const IdType m_id{};
    const Action m_enterAction;
    const Action m_exitAction;
};

template <typename Id>
class StateBase<Id>::Builder final {
public:
    [[nodiscard]] auto set_id(IdType t_id) noexcept -> Builder&;
    [[nodiscard]] auto on_enter(Action&& t_callback) noexcept -> Builder&;
    [[nodiscard]] auto on_exit(Action&& t_callback) noexcept -> Builder&;

    [[nodiscard]] explicit(false) operator StateBase<Id>() noexcept;
    [[nodiscard]] auto build() noexcept -> StateBase<Id>;

private:
    IdType m_id;
    StateBase<Id>::Action m_enterAction;
    StateBase<Id>::Action m_exitAction;
};

template <typename Id>
auto StateBase<Id>::id() const noexcept -> IdType {
    return m_id;
}

template <typename Id>
void StateBase<Id>::enter() const noexcept {
    if (m_enterAction) {
        m_enterAction();
    }
}

template <typename Id>
void StateBase<Id>::exit() const noexcept {
    if (m_exitAction) {
        m_exitAction();
    }
}

template <typename Id>
auto StateBase<Id>::create() noexcept -> Builder {
    return Builder{};
}

template <typename Id>
StateBase<Id>::StateBase(const IdType t_id,
                         Action&& t_enterAction,
                         Action&& t_exitAction) noexcept
    : m_id{ t_id },
      m_enterAction{ std::move(t_enterAction) },
      m_exitAction{ std::move(t_exitAction) } {}

template <typename Id>
auto StateBase<Id>::Builder::set_id(IdType t_id) noexcept -> Builder& {
    m_id = t_id;
    return *this;
}

template <typename Id>
auto StateBase<Id>::Builder::on_enter(Action&& t_callback) noexcept
    -> Builder& {
    m_enterAction = std::move(t_callback);
    return *this;
}

template <typename Id>
auto StateBase<Id>::Builder::on_exit(Action&& t_callback) noexcept -> Builder& {
    m_exitAction = std::move(t_callback);
    return *this;
}

template <typename Id>
StateBase<Id>::Builder::operator StateBase<Id>() noexcept {
    return build();
}

template <typename Id>
auto StateBase<Id>::Builder::build() noexcept -> StateBase<Id> {
    return StateBase<Id>{ m_id,
                          std::move(m_enterAction),
                          std::move(m_exitAction) };
}

using State = StateBase<uint32_t>;

}   // namespace fw
