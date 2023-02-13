#pragma once

#include <functional>
#include <vector>

#include "Controller.hpp"
#include "Stage.hpp"

namespace app {

class Schedule final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

private:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Controller = Controller<Schedule>;

public:
    using Stage = BasicStage<Controller>;
    using StageContainer = std::vector<Stage>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Schedule(StageContainer&& t_stages = {}) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template<class AppView>
    explicit(false) operator std::function<void(AppView)>();

    template<class AppView>
    void execute(AppView t_app);

    [[nodiscard]] auto running() const noexcept -> bool;
    void quit() noexcept;

private:
    template<class AppView>
    void advance(AppView t_app, Controller t_controller);

    ///-------------///
    ///  Variables  ///
    ///-------------///
    bool m_running = true;
    StageContainer m_stages;
};

class Schedule::Builder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = Schedule;

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

}   // namespace app

#include "Schedule.inl"
