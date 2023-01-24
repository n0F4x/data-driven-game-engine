#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

#include "engine/State.hpp"

class StateMachine {
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
    void start();
    void exit() noexcept;
    void transition() noexcept;
    void transition_to(Id state) noexcept;
    void transition_to_prev() noexcept;
    void add_state(State&& state);

    ///------------------///
    ///  Static helpers  ///
    ///------------------///
    [[nodiscard]] static auto running(const StateMachine& machine) noexcept
        -> bool;

    private:
    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    std::unique_ptr<std::mutex> transitionLock = std::make_unique<std::mutex>();
    std::unordered_map<Id, const State> states;
    gsl::not_null<const State*> nextState = State::invalid_state();
    gsl::not_null<const State*> currentState = State::invalid_state();
    gsl::not_null<const State*> prevState = State::invalid_state();
};
