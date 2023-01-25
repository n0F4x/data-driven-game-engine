#include "engine/App.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

#include "engine/Controller.hpp"
#include "framework/Scene.hpp"
#include "engine/Stage.hpp"
#include "framework/State.hpp"

void App::run() {
    std::cout << name << " is running...\n";

    stateMachine.start();

    if (!Scheduler::empty(scheduler)) {
        Controller controller{ stateMachine };

        while (StateMachine::running(stateMachine)) {
            scheduler.iterate(controller);

            stateMachine.transition();
        }
    }

    stateMachine.transition();
}

[[nodiscard]] auto App::create() noexcept -> Builder {
    return Builder{};
}

[[nodiscard]] auto App::Builder::set_name(std::string_view new_name) noexcept
    -> Self {
    draft().name = new_name;

    return std::move(*this);
}

[[nodiscard]] auto App::Builder::add_state(State&& state) -> Self {
    draft().stateMachine.add_state(std::move(state));

    return std::move(*this);
}

[[nodiscard]] auto App::Builder::add_stage(Stage&& stage) -> Self {
    draft().scheduler.add_stage(std::move(stage));

    return std::move(*this);
}
