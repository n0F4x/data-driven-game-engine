#include "engine/App.hpp"

#include <algorithm>
#include <functional>
#include <iostream>

#include "engine/Controller.hpp"
#include "engine/Stage.hpp"
#include "framework/Scene.hpp"
#include "framework/State.hpp"

namespace engine {

void App::run() {
    std::cout << m_name << " is running...\n";

    m_stateMachine.start();

    if (!Scheduler::empty(m_scheduler)) {
        Controller controller{ m_stateMachine };

        while (fw::StateMachine::running(m_stateMachine)) {
            m_scheduler.iterate(controller);

            m_stateMachine.transition();
        }
    }

    m_stateMachine.transition();
}

[[nodiscard]] auto App::create() noexcept -> Builder {
    return Builder{};
}

[[nodiscard]] auto App::Builder::set_name(std::string_view t_name) noexcept
    -> Self {
    draft().m_name = t_name;

    return std::move(*this);
}

[[nodiscard]] auto App::Builder::add_state(fw::State&& t_state) -> Self {
    draft().m_stateMachine.add_state(std::move(t_state));

    return std::move(*this);
}

[[nodiscard]] auto App::Builder::add_stage(Stage&& t_stage) -> Self {
    draft().m_scheduler.add_stage(std::move(t_stage));

    return std::move(*this);
}

}   // namespace engine
