#pragma once

#include <functional>
#include <utility>

#include <gsl/pointers>

namespace fw {

template <typename TId>
class BasicState final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Id = TId;
    using Action = std::function<void()>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicState(Id t_id = {},
                                      Action&& t_enterAction = {},
                                      Action&& t_exitAction = {}) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto id() const noexcept -> Id;
    void enter() const noexcept;
    void exit() const noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Id m_id{};
    Action m_enterAction;
    Action m_exitAction;
};

template <typename TId>
class BasicState<TId>::Builder final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicState<TId>;
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product() noexcept;
    [[nodiscard]] auto build() noexcept -> Product;

    [[nodiscard]] auto set_id(Id t_id) noexcept -> Builder&;
    [[nodiscard]] auto on_enter(Action&& t_callback) noexcept -> Builder&;
    [[nodiscard]] auto on_exit(Action&& t_callback) noexcept -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Id m_id;
    Action m_enterAction;
    Action m_exitAction;
};

template <typename TId>
BasicState<TId>::BasicState(const Id t_id,
                            Action&& t_enterAction,
                            Action&& t_exitAction) noexcept
    : m_id{ t_id },
      m_enterAction{ std::move(t_enterAction) },
      m_exitAction{ std::move(t_exitAction) } {}

template <typename TId>
auto BasicState<TId>::id() const noexcept -> Id {
    return m_id;
}

template <typename TId>
void BasicState<TId>::enter() const noexcept {
    if (m_enterAction) {
        m_enterAction();
    }
}

template <typename TId>
void BasicState<TId>::exit() const noexcept {
    if (m_exitAction) {
        m_exitAction();
    }
}

template <typename TId>
BasicState<TId>::Builder::operator Product() noexcept {
    return build();
}

template <typename TId>
auto BasicState<TId>::Builder::build() noexcept -> Product {
    return Product{ m_id, std::move(m_enterAction), std::move(m_exitAction) };
}

template <typename TId>
auto BasicState<TId>::Builder::set_id(Id t_id) noexcept -> Builder& {
    m_id = t_id;
    return *this;
}

template <typename TId>
auto BasicState<TId>::Builder::on_enter(Action&& t_callback) noexcept
    -> Builder& {
    m_enterAction = std::move(t_callback);
    return *this;
}

template <typename TId>
auto BasicState<TId>::Builder::on_exit(Action&& t_callback) noexcept
    -> Builder& {
    m_exitAction = std::move(t_callback);
    return *this;
}

using State = BasicState<uint32_t>;

}   // namespace fw
