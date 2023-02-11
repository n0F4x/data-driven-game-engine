#pragma once

#include <memory>
#include <vector>

#include <gsl/pointers>

#include "app/config/config.hpp"
#include "Controller.hpp"
#include "engine/state_machine/StateMachine.hpp"
#include "Renderer.hpp"
#include "Schedule.hpp"

namespace app {

class App final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Renderer = Renderer;
    using Schedule = BasicSchedule<Controller&>;
    using StateMachine =
        engine::BasicStateMachine<engine::BasicState<config::StateId>>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit App(std::string_view t_name,
                               Renderer&& t_renderer,
                               Schedule&& t_schedule,
                               StateMachine&& t_stateMachine) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run();

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string m_name;
    Renderer m_renderer;
    Schedule m_schedule;
    StateMachine m_stateMachine;
};

class App::Builder final {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator App() noexcept;
    [[nodiscard]] auto build() noexcept -> App;

    [[nodiscard]] auto set_name(std::string_view t_name) noexcept -> Builder&;
    [[nodiscard]] auto set_renderer(Renderer&& t_renderer) -> Builder&;
    [[nodiscard]] auto set_schedule(Schedule&& t_schedule) -> Builder&;
    [[nodiscard]] auto set_state_machine(StateMachine&& t_stateMachine)
        -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string_view m_name = "App";
    Renderer m_renderer;
    Schedule m_schedule;
    StateMachine m_stateMachine;
};

}   // namespace app
