#include "engine/Scheduler.hpp"

#include <algorithm>
#include <functional>
#include <future>
#include <vector>

#include "engine/Stage.hpp"
#include "framework/SceneGraph.hpp"

namespace engine {

Scheduler::Scheduler(std::function<fw::Scene()>&& t_sceneMaker)
    : m_sceneMaker{ std::move(t_sceneMaker) } {}

void Scheduler::iterate(Controller& t_controller) {
    std::swap(m_previousScene, m_scene);

    auto stagesFuture = std::async(std::launch::async, [this, &t_controller] {
        std::ranges::for_each(m_stages, [&t_controller](auto& t_stage) {
            t_stage.run(t_controller);
        });
    });

    auto renderFuture = std::async(std::launch::async, fw::Scene::render,
                                   std::ref(m_previousScene));

    m_scene = m_sceneMaker();

    // throw potential exception from threads
    stagesFuture.get();
    renderFuture.get();
}

void Scheduler::add_stage(Stage&& t_stage) {
    if (!Stage::empty(t_stage)) {
        m_stages.push_back(std::move(t_stage));
    }
}

[[nodiscard]] auto Scheduler::empty(Scheduler& t_scheduler) -> bool {
    return std::ranges::empty(t_scheduler.m_stages);
}

}   // namespace engine
