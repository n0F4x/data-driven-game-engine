#pragma once

#include <vector>

#include "app/core/schedule.hpp"
#include "Controller.hpp"
#include "Stage.hpp"

namespace app {

template <class AppViewType>
class Schedule final : public ScheduleInterface<AppViewType> {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

private:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Controller = Controller<Schedule<AppViewType>>;

public:
    using AppView = AppViewType;
    using Stage = BasicStage<Controller>;
    using StageContainer = std::vector<Stage>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Schedule(StageContainer&& t_stages = {}) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void execute(AppView t_app) override;

    [[nodiscard]] auto running() const noexcept -> bool;
    void quit() noexcept;

private:
    void advance(AppView t_app, Controller t_controller);

    ///-------------///
    ///  Variables  ///
    ///-------------///
    bool m_running = true;
    StageContainer m_stages;
};

template <class AppViewType>
class Schedule<AppViewType>::Builder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = Schedule<AppViewType>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product() noexcept;
    [[nodiscard]] auto build() noexcept -> Product;

    [[nodiscard]] auto add_stage(Stage&& t_stage) -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StageContainer m_stages;
};

}   // namespace engine

#include "Schedule.inl"
