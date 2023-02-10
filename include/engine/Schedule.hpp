#pragma once

#include <vector>

#include "engine/Controller.hpp"
#include "engine/Stage.hpp"

namespace engine {

template <class ControllerType>
class BasicSchedule final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Controller = ControllerType;
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
    void execute(Controller t_controller);

private:
    void advance(Controller t_controller);

    ///-------------///
    ///  Variables  ///
    ///-------------///
    StageContainer m_stages;
};

template <class ControllerType>
class BasicSchedule<ControllerType>::Builder {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicSchedule<ControllerType>;

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
