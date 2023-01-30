#pragma once

#include <memory>
#include <vector>

#include <gsl/pointers>

#include "engine/config/config.hpp"
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
    using SceneGraph = fw::SceneGraph;
    using Schedule = BasicSchedule<Controller&>;
    using StateMachine =
        fw::fsm::BasicStateMachine<fw::BasicState<config::StateId>>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit App(std::string_view t_name,
                               SceneGraph&& t_sceneGraph,
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
    SceneGraph m_sceneGraph;
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
    [[nodiscard]] auto set_scene_graph(SceneGraph&& t_sceneGraph) -> Builder&;
    [[nodiscard]] auto set_schedule(Schedule&& t_schedule) -> Builder&;
    [[nodiscard]] auto set_state_machine(StateMachine&& t_stateMachine)
        -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::string_view m_name = "App";
    SceneGraph m_sceneGraph;
    Schedule m_schedule;
    StateMachine m_stateMachine;
};

}   // namespace engine
