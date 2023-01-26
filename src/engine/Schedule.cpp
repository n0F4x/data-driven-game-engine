#include "engine/Schedule.hpp"

#include <algorithm>
#include <functional>
#include <future>
#include <vector>

#include "engine/Stage.hpp"
#include "framework/SceneGraph.hpp"

namespace engine {

Schedule::Schedule(std::function<fw::Scene()>&& t_sceneMaker)
    : m_sceneMaker{ std::move(t_sceneMaker) } {}

[[nodiscard]] auto Schedule::empty() const noexcept -> bool {
    return std::ranges::empty(m_stages);
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

    m_scene = m_sceneMaker();

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
