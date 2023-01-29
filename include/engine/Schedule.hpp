#pragma once

#include <vector>

#include "engine/Controller.hpp"
#include "engine/Stage.hpp"
#include "framework/Scene.hpp"

namespace engine {

template <class TController>
class BasicSchedule final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Controller = TController;
    using Stage = BasicStage<Controller>;
    using StageContainer = std::vector<Stage>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicSchedule(
        StageContainer&& t_stages = {}) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run(Controller t_controller);

private:
    void iterate(Controller t_controller);

    ///-------------///
    ///  Variables  ///
    ///-------------///
    StageContainer m_stages;
    fw::Scene m_previousScene;
    fw::Scene m_scene;
};

template <class TController>
class BasicSchedule<TController>::Builder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicSchedule<TController>;

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

using Schedule = BasicSchedule<Controller&>;

}   // namespace engine

#include "engine/Schedule.inl"
