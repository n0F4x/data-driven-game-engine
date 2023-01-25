#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

#include "framework/State.hpp"

namespace fw {

class StateMachine final {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] StateMachine() = default;
    [[nodiscard]] StateMachine(const StateMachine&) = delete;
    [[nodiscard]] StateMachine(StateMachine&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void start();
    void exit() noexcept;
    void transition() noexcept;
    void transition_to(config::Id t_stateId) noexcept;
    void transition_to_previous() noexcept;
    void add_state(State&& t_state);

private:
    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    std::unique_ptr<std::mutex> m_transitionLock =
        std::make_unique<std::mutex>();
    std::unordered_map<config::Id, const State> m_states;
    gsl::not_null<const State*> m_nextState = State::invalid_state();
    gsl::not_null<const State*> m_currentState = State::invalid_state();
    gsl::not_null<const State*> m_prevState = State::invalid_state();
};

}   // namespace fw
