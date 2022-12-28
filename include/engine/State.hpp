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

    constexpr static auto empty_action() noexcept -> void {}

public:
    template<State::Id id>
        requires(id != 0)
    [[nodiscard]] constexpr static auto create() noexcept;

    [[nodiscard]] static State& invalid_state() noexcept {
        static State invalid_state{};
        return invalid_state;
    }

    constexpr auto entered() const noexcept {
        onEnter();
    }
    constexpr auto exited() const noexcept {
        onExit();
    }

    [[nodiscard]] constexpr auto get_id() const noexcept {
        return id;
    }

private:
    [[nodiscard]] constexpr explicit State(Id id = {}) noexcept : id{ id } {}

    Id id{};

    Action onEnter = empty_action;
    Action onExit = empty_action;
};

[[nodiscard]] constexpr auto is_valid(const State& state) noexcept {
    return state.get_id() != 0;
}


class State::Builder final : public BuilderBase<State> {
public:
    using BuilderBase<State>::BuilderBase;

    [[nodiscard]] constexpr auto on_enter(Action&& callback) noexcept {
        draft().onEnter = std::move(callback);

        return std::move(*this);
    }

    [[nodiscard]] constexpr auto on_exit(Action&& callback) noexcept {
        draft().onExit = std::move(callback);

        return std::move(*this);
    }
};


template<State::Id id>
    requires(id != 0)
[[nodiscard]] constexpr auto State::create() noexcept {
    return Builder{ id };
}
