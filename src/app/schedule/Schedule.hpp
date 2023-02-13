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

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Schedule(std::vector<Stage>&& t_stages = {}) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template<class AppView>
    explicit(false) operator std::function<void(AppView)>();

    template<class AppView>
    void execute(AppView t_app);

    void quit() noexcept;

private:
    template<class AppView>
    void advance(AppView t_app, Controller t_controller);

    ///-------------///
    ///  Variables  ///
    ///-------------///
    bool m_running = true;
    std::vector<Stage> m_stages;
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
    std::vector<Stage> m_stages;
};

}   // namespace app

#include "Controller.inl"
#include "Stage.inl"

#include "Schedule.inl"
