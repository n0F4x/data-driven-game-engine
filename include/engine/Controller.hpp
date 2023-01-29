#pragma once

#include "framework/SceneGraph.hpp"
#include "framework/StateMachine.hpp"

namespace engine {

class Controller final {
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using SceneGraph = fw::SceneGraph;
    using StateMachine = fw::fsm::StateMachine;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Controller(SceneGraph& t_sceneGraph,
                                      StateMachine& t_stateMachine) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void quit() noexcept;
    [[nodiscard]] auto sceneGraph() noexcept -> SceneGraph&;
    [[nodiscard]] auto stateMachine() noexcept -> StateMachine&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    bool m_running = true;
    SceneGraph& m_sceneGraph;
    StateMachine& m_stateMachine;
};

}   // namespace engine
