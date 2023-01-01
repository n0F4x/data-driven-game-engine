#include "engine/App.hpp"

#include <iostream>
#include <format>
#include <algorithm>
#include <functional>

#include "engine/Controller.hpp"
#include "engine/State.hpp"
#include "engine/Stage.hpp"


void App::run() {
    std::cout << std::format("{} is running...\n", name);

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


[[nodiscard]] auto App::Builder::set_name(std::string_view new_name) noexcept -> Self {
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

[[nodiscard]] auto App::Builder::add_render_stage(Stage&& stage) -> Self {
    draft().scheduler.add_render_stage(std::move(stage));

    return std::move(*this);
}
