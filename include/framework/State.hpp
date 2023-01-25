#pragma once

#include <functional>
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
    using Action = std::function<void()>;

private:
    class Builder;
    friend BuilderBase<State>;

    friend App;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] State(const State&) = delete;
    [[nodiscard]] State(State&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto get_id() const noexcept -> Id;
    void entered() const noexcept;
    void exited() const noexcept;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    template <Id id>
        requires(id != 0)
    [[nodiscard]] static auto create() noexcept -> Builder;
    [[nodiscard]] static auto invalid_state() noexcept
        -> gsl::not_null<const State*>;
    [[nodiscard]] static auto invalid(const State& state) noexcept -> bool;

private:
    [[nodiscard]] explicit State(Id id = {}) noexcept : id{ id } {}

    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    static const State s_invalid_state;

    Id id{};
    Action onEnter;
    Action onExit;
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
    [[nodiscard]] auto on_enter(Action&& callback) noexcept -> Self;
    [[nodiscard]] auto on_exit(Action&& callback) noexcept -> Self;
};

template <Id id>
    requires(id != 0)
auto State::create() noexcept -> State::Builder {
    return Builder{ id };
}
