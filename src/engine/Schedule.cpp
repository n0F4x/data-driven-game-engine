#include "engine/Schedule.hpp"

#include <algorithm>
#include <functional>
#include <future>
#include <vector>

#include "engine/App.hpp"
#include "engine/Controller.hpp"
#include "engine/Stage.hpp"
#include "framework/SceneGraph.hpp"

namespace engine {

Schedule::Schedule(App& t_app) : m_app{ t_app } {}

void Schedule::run() {
    m_app.m_stateMachine.start();

    Controller controller{ m_app.m_stateMachine };

    while (m_app.m_stateMachine.running()) {
        iterate(controller);

        m_app.m_stateMachine.transition();
    }

    m_app.m_stateMachine.transition();
}

void Schedule::iterate(Controller& t_controller) {
    std::swap(m_previousScene, m_scene);

    auto stagesFuture = std::async(std::launch::async, [this, &t_controller] {
        std::ranges::for_each(m_stages, [&t_controller](auto& t_stage) {
            t_stage.run(t_controller);
        });
    });

    auto renderFuture = std::async(
        std::launch::async, &fw::Scene::render, std::ref(m_previousScene));

    m_scene = m_app.m_sceneGraph.make_scene();

    // throw potential exception from threads
    stagesFuture.get();
    renderFuture.get();
}

void Schedule::add_stage(Stage&& t_stage) {
    if (!t_stage.empty()) {
        m_stages.push_back(std::move(t_stage));
    }
}

}   // namespace engine
