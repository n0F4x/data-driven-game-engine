#pragma once

#include <type_traits>
#include <utility>

#include <gsl/pointers>

#include <entt/entt.hpp>

#include "patterns/builder/helper.hpp"


class State final {
public:
    using Action = gsl::not_null<void(*)()>;
    using Id = entt::hashed_string::hash_type;

private:
    class Builder;
    friend BuilderBase<State>;

    constexpr static void empty_action() noexcept { /*empty by default*/ }

public:
    [[nodiscard]] constexpr State(const State&) = delete;
    [[nodiscard]] constexpr State(State&&) noexcept = default;

    template<State::Id id>
        requires(id != 0)
    [[nodiscard]] constexpr static auto create() noexcept;

    [[nodiscard]] constexpr static gsl::not_null<const State*> invalid_state() noexcept {
        return &s_invalid_state;
    }

    [[nodiscard]] constexpr static auto invalid(const State& state) noexcept {
        return state.id == 0;
    }

    [[nodiscard]] constexpr auto get_id() const noexcept {
        return id;
    }

    constexpr void entered() const noexcept {
        onEnter();
    }
    constexpr void exited() const noexcept {
        onExit();
    }

private:
    [[nodiscard]] constexpr explicit State(Id id = {}) noexcept : id{ id } {}

    static const State s_invalid_state;

    Id id{};

    Action onEnter = empty_action;
    Action onExit = empty_action;
};

constexpr const State State::s_invalid_state;


class State::Builder final : public BuilderBase<State> {
public:
    using BuilderBase<State>::BuilderBase;

    [[nodiscard]] constexpr auto on_enter(Action callback) noexcept {
        draft().onEnter = callback;

        return std::move(*this);
    }

    [[nodiscard]] constexpr auto on_exit(Action callback) noexcept {
        draft().onExit = callback;

        return std::move(*this);
    }
};


template<State::Id id>
    requires(id != 0)
[[nodiscard]] constexpr auto State::create() noexcept {
    return Builder{ id };
}
