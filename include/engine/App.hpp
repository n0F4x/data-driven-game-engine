#pragma once

#include <memory>
#include <vector>

#include <gsl/pointers>

#include "engine/Controller.hpp"
#include "engine/Schedule.hpp"
#include "framework/SceneGraph.hpp"
#include "framework/StateMachine.hpp"

namespace engine {

class App final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using SceneGraphType = fw::SceneGraph;
    using ScheduleType = BasicSchedule<Controller>;
    using StateMachineType = fw::fsm::StateMachine;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit App(std::string_view t_name,
                               SceneGraphType&& t_sceneGraph,
                               ScheduleType&& t_schedule,
                               StateMachineType&& t_stateMachine) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run();

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string m_name;
    SceneGraphType m_sceneGraph;
    ScheduleType m_schedule;
    StateMachineType m_stateMachine;
};

class App::Builder final {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator App() noexcept;
    [[nodiscard]] auto build() noexcept -> App;

    [[nodiscard]] auto set_name(std::string_view t_name) noexcept -> Builder&;
    [[nodiscard]] auto set_scene_graph(SceneGraphType&& t_sceneGraph)
        -> Builder&;
    [[nodiscard]] auto set_schedule(Schedule&& t_schedule) -> Builder&;
    [[nodiscard]] auto set_state_machine(StateMachineType&& t_stateMachine)
        -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string_view m_name = "App";
    SceneGraphType m_sceneGraph;
    Schedule m_schedule;
    StateMachineType m_stateMachine;
};

}   // namespace engine
