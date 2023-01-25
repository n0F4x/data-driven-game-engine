#pragma once

#include <utility>

#include <gsl/pointers>

#include "common/patterns/builder/helper.hpp"
#include "engine/config/id.hpp"

class App;

class State final {
    ///----------------///
    ///  Member types  ///
    ///----------------///

public:
    using Action = gsl::not_null<void (*)()>;

private:
    class Builder;
    friend BuilderBase<State>;

    friend App;

    constexpr static void empty_action() noexcept { /*empty by default*/
    }

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] constexpr State(const State&) = delete;
    [[nodiscard]] constexpr State(State&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] constexpr auto get_id() const noexcept;
    constexpr void entered() const noexcept;
    constexpr void exited() const noexcept;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    template <Id id>
        requires(id != 0)
    [[nodiscard]] constexpr static auto create() noexcept;
    [[nodiscard]] constexpr static auto invalid_state() noexcept
        -> gsl::not_null<const State*>;
    [[nodiscard]] constexpr static auto invalid(const State& state) noexcept;

private:
    [[nodiscard]] constexpr explicit State(Id id = {}) noexcept : id{ id } {}

    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    static const State s_invalid_state;

    Id id{};
    Action onEnter = empty_action;
    Action onExit = empty_action;
};

class State::Builder final : public BuilderBase<State> {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    using BuilderBase<State>::BuilderBase;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] constexpr auto on_enter(Action callback) noexcept;
    [[nodiscard]] constexpr auto on_exit(Action callback) noexcept;
};

/// ////////////////////// ///
///     IMPLEMENTATION     ///
/// ////////////////////// ///

constexpr inline State State::s_invalid_state;

template <Id id>
    requires(id != 0)
[[nodiscard]] constexpr auto State::create() noexcept {
    return Builder{ id };
}

[[nodiscard]] constexpr gsl::not_null<const State*>
State::invalid_state() noexcept {
    return &s_invalid_state;
}

[[nodiscard]] constexpr auto State::invalid(const State& state) noexcept {
    return state.id == 0;
}

[[nodiscard]] constexpr auto State::get_id() const noexcept {
    return id;
}

constexpr void State::entered() const noexcept {
    onEnter();
}

constexpr void State::exited() const noexcept {
    onExit();
}

[[nodiscard]] constexpr auto
State::Builder::on_enter(Action callback) noexcept {
    draft().onEnter = callback;

    return std::move(*this);
}

[[nodiscard]] constexpr auto State::Builder::on_exit(Action callback) noexcept {
    draft().onExit = callback;

    return std::move(*this);
}
