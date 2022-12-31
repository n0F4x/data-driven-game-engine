#include "engine/Controller.hpp"

#include "engine/App.hpp"
#include "engine/State.hpp"


void Controller::quit() noexcept {
    *app.running = false;
    app.nextState = State::invalid_state();
}

void Controller::transition_to(Id to) noexcept {
    if (app.nextState == app.currentState)
        if (auto iter{ app.states.find(to) }; iter != app.states.end())
            app.nextState = &iter->second;
}

void Controller::transition_to_prev() noexcept {
    if (app.nextState == app.currentState)
        app.nextState = app.prevState;
}

