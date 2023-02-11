#pragma once

#include <memory>
#include <vector>

#include <gsl/pointers>

#include "app/config/config.hpp"
#include "Controller.hpp"
#include "engine/state_machine/StateMachine.hpp"
#include "Schedule.hpp"

namespace app {

template <class RendererType, class StateMachineType>
class BasicApp final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Renderer = RendererType;
    using Schedule = BasicSchedule<Controller&>;
    using StateMachine = StateMachineType;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicApp(std::string_view t_name,
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

template <class RendererType, class StateMachineType>
class BasicApp<RendererType, StateMachineType>::Builder final {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator BasicApp() noexcept;
    [[nodiscard]] auto build() noexcept -> BasicApp;

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

using App = BasicApp<config::Renderer, config::StateMachine>;

}   // namespace app

#include "App.inl"
