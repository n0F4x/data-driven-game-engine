#pragma once

#include <functional>

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

using State = BasicState<uint32_t>;

}   // namespace fw

#include "framework/State.inl"
