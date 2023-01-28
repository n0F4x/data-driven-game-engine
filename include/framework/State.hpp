#pragma once

#include <functional>
#include <utility>

#include <gsl/pointers>

namespace fw {

template <typename Id>
class BasicState final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using IdType = Id;
    using Action = std::function<void()>;

private:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicState(IdType t_id = {},
                                      Action&& t_enterAction = {},
                                      Action&& t_exitAction = {}) noexcept;

public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto id() const noexcept -> IdType;
    void enter() const noexcept;
    void exit() const noexcept;

    ///--------------------///
    ///  Static functions  ///
    ///--------------------///
    [[nodiscard]] static auto create() noexcept -> Builder;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    const IdType m_id{};
    const Action m_enterAction;
    const Action m_exitAction;
};

template <typename Id>
class BasicState<Id>::Builder final {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator BasicState<Id>() noexcept;
    [[nodiscard]] auto build() noexcept -> BasicState<Id>;

    [[nodiscard]] auto set_id(IdType t_id) noexcept -> Builder&;
    [[nodiscard]] auto on_enter(Action&& t_callback) noexcept -> Builder&;
    [[nodiscard]] auto on_exit(Action&& t_callback) noexcept -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    IdType m_id;
    BasicState<Id>::Action m_enterAction;
    BasicState<Id>::Action m_exitAction;
};

template <typename Id>
auto BasicState<Id>::id() const noexcept -> IdType {
    return m_id;
}

template <typename Id>
void BasicState<Id>::enter() const noexcept {
    if (m_enterAction) {
        m_enterAction();
    }
}

template <typename Id>
void BasicState<Id>::exit() const noexcept {
    if (m_exitAction) {
        m_exitAction();
    }
}

template <typename Id>
auto BasicState<Id>::create() noexcept -> Builder {
    return Builder{};
}

template <typename Id>
BasicState<Id>::BasicState(const IdType t_id,
                           Action&& t_enterAction,
                           Action&& t_exitAction) noexcept
    : m_id{ t_id },
      m_enterAction{ std::move(t_enterAction) },
      m_exitAction{ std::move(t_exitAction) } {}

template <typename Id>
auto BasicState<Id>::Builder::set_id(IdType t_id) noexcept -> Builder& {
    m_id = t_id;
    return *this;
}

template <typename Id>
auto BasicState<Id>::Builder::on_enter(Action&& t_callback) noexcept
    -> Builder& {
    m_enterAction = std::move(t_callback);
    return *this;
}

template <typename Id>
auto BasicState<Id>::Builder::on_exit(Action&& t_callback) noexcept
    -> Builder& {
    m_exitAction = std::move(t_callback);
    return *this;
}

template <typename Id>
BasicState<Id>::Builder::operator BasicState<Id>() noexcept {
    return build();
}

template <typename Id>
auto BasicState<Id>::Builder::build() noexcept -> BasicState<Id> {
    return BasicState<Id>{ m_id,
                           std::move(m_enterAction),
                           std::move(m_exitAction) };
}

using State = BasicState<uint32_t>;

}   // namespace fw
