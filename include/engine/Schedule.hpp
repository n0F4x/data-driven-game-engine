#pragma once

#include <algorithm>
#include <functional>
#include <future>
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

template <class TController>
BasicSchedule<TController>::BasicSchedule(StageContainer&& t_stages) noexcept
    : m_stages{ std::move(t_stages) } {}

template <class TController>
void BasicSchedule<TController>::run(Controller t_controller) {
    while (t_controller.running()) {
        iterate(t_controller);

        t_controller.stateMachine().transition();
    }
}

template <class TController>
void BasicSchedule<TController>::iterate(Controller t_controller) {
    std::swap(m_previousScene, m_scene);

    auto stagesFuture = std::async(std::launch::async, [this, &t_controller] {
        std::ranges::for_each(m_stages, [&t_controller](auto& t_stage) {
            t_stage.run(t_controller);
        });
    });

    auto renderFuture = std::async(
        std::launch::async, &fw::Scene::render, std::ref(m_previousScene));

    m_scene = t_controller.sceneGraph().make_scene();

    // throw potential exception from threads
    stagesFuture.get();
    renderFuture.get();
}

template <class TController>
BasicSchedule<TController>::Builder::operator Product() noexcept {
    return build();
}

template <class TController>
auto BasicSchedule<TController>::Builder::build() noexcept -> Product {
    return BasicSchedule{ std::move(m_stages) };
}

template <class TController>
auto BasicSchedule<TController>::Builder::add_stage(Stage&& t_stage)
    -> Builder& {
    m_stages.push_back(std::move(t_stage));
    return *this;
}

using Schedule = BasicSchedule<Controller&>;

}   // namespace engine
