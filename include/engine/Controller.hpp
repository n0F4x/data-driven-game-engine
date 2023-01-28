#pragma once

#include "framework/SceneGraph.hpp"
#include "framework/StateMachine.hpp"

namespace engine {

class Controller final {
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using SceneGraphType = fw::SceneGraph;
    using StateMachineType = fw::fsm::StateMachine;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Controller(SceneGraphType& t_sceneGraph,
                                      StateMachineType& t_stateMachine) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void quit() noexcept;
    [[nodiscard]] auto sceneGraph() noexcept -> SceneGraphType&;
    [[nodiscard]] auto stateMachine() noexcept -> StateMachineType&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    bool m_running = true;
    SceneGraphType& m_sceneGraph;
    StateMachineType& m_stateMachine;
};

}   // namespace engine
