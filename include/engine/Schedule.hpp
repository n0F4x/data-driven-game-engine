#pragma once

#include <algorithm>
#include <functional>
#include <future>
#include <vector>

#include "engine/Controller.hpp"
#include "engine/Stage.hpp"
#include "framework/Scene.hpp"

namespace engine {

template <class Controller>
class BasicSchedule final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using ControllerType = Controller;
    using StageType = BasicStage<ControllerType>;
    using StageContainerType = std::vector<StageType>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicSchedule(
        StageContainerType&& t_stages = {}) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void run(ControllerType& t_controller);

private:
    void iterate(ControllerType& t_controller);

    ///-------------///
    ///  Variables  ///
    ///-------------///
    StageContainerType m_stages;
    fw::Scene m_previousScene;
    fw::Scene m_scene;
};

template <class Controller>
class BasicSchedule<Controller>::Builder {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator BasicSchedule<Controller>() noexcept;
    [[nodiscard]] auto build() noexcept -> BasicSchedule<Controller>;

    [[nodiscard]] auto add_stage(StageType&& t_stage) -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StageContainerType m_stages;
};

template <class Controller>
BasicSchedule<Controller>::BasicSchedule(StageContainerType&& t_stages) noexcept
    : m_stages{ std::move(t_stages) } {}

template <class Controller>
void BasicSchedule<Controller>::run(ControllerType& t_controller) {
    while (t_controller.running()) {
        iterate(t_controller);

        t_controller.transition();
    }
}

template <class Controller>
void BasicSchedule<Controller>::iterate(ControllerType& t_controller) {
    std::swap(m_previousScene, m_scene);

    auto stagesFuture = std::async(std::launch::async, [this, &t_controller] {
        std::ranges::for_each(m_stages, [&t_controller](auto& t_stage) {
            t_stage.run(t_controller);
        });
    });

    auto renderFuture = std::async(
        std::launch::async, &fw::Scene::render, std::ref(m_previousScene));

    m_scene = t_controller.make_scene();

    // throw potential exception from threads
    stagesFuture.get();
    renderFuture.get();
}

template <class Controller>
BasicSchedule<Controller>::Builder::operator BasicSchedule<Controller>() noexcept {
    return build();
}

template <class Controller>
auto BasicSchedule<Controller>::Builder::build() noexcept -> BasicSchedule<Controller> {
    return BasicSchedule{ std::move(m_stages) };
}

template <class Controller>
auto BasicSchedule<Controller>::Builder::add_stage(StageType&& t_stage)
    -> Builder& {
    m_stages.push_back(std::move(t_stage));
    return *this;
}

using Schedule = BasicSchedule<Controller>;

}   // namespace engine
