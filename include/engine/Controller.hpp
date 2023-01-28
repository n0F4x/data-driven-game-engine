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
    [[nodiscard]] auto make_scene() const noexcept -> SceneGraphType::SceneType;
    [[nodiscard]] auto running() const noexcept -> bool;
    void quit() noexcept;
    void transition() noexcept;
    void transition_to(StateMachineType::StateIdType t_nextStateId) noexcept;
    void transition_to_prev() noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    SceneGraphType& m_sceneGraph;
    StateMachineType& m_stateMachine;
};

}   // namespace engine
