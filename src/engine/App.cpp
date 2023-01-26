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

    m_schedule.run();
}

[[nodiscard]] auto App::Builder::set_name(std::string_view t_name) noexcept
    -> Builder& {
    draft().m_name = t_name;

    return *this;
}

[[nodiscard]] auto
App::Builder::add_state(fw::State&& t_state, bool t_setAsInitialState)
    -> Builder& {
    draft().m_stateMachine.add_state(std::move(t_state), t_setAsInitialState);

    return *this;
}

[[nodiscard]] auto App::Builder::add_stage(Stage&& t_stage) -> Builder& {
    draft().m_schedule.add_stage(std::move(t_stage));

    return *this;
}

}   // namespace engine
