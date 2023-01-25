#pragma once

#include <functional>
#include <utility>

#include <gsl/pointers>

#include "common/patterns/builder/helper.hpp"
#include "engine/config/config.hpp"

namespace fw {

class State final {
    ///----------------///
    ///  Member types  ///
    ///----------------///

public:
    using Action = std::function<void()>;

private:
    class Builder;
    friend BuilderBase<State>;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] State(const State&) = delete;
    [[nodiscard]] State(State&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto id() const noexcept -> config::Id;
    [[nodiscard]] auto invalid() const noexcept -> bool;
    void entered() const noexcept;
    void exited() const noexcept;

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    template <config::Id t_id>
        requires(t_id != 0)
    [[nodiscard]] static auto create() noexcept -> Builder;
    [[nodiscard]] static auto invalid_state() noexcept
        -> gsl::not_null<const State*>;

private:
    [[nodiscard]] explicit State(config::Id t_id = {}) noexcept;

    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    static const State s_invalidState;

    config::Id m_id{};
    Action m_enterAction;
    Action m_exitAction;
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
    [[nodiscard]] auto on_enter(Action&& t_callback) noexcept -> Self;
    [[nodiscard]] auto on_exit(Action&& t_callback) noexcept -> Self;
};

template <config::Id t_id>
    requires(t_id != 0)
auto State::create() noexcept -> State::Builder {
    return Builder{ t_id };
}

}   // namespace fw
