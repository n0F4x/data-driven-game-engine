#include "engine/Scheduler.hpp"

#include <algorithm>
#include <functional>
#include <future>
#include <vector>

#include "framework/SceneGraph.hpp"
#include "engine/Stage.hpp"

Scheduler::Scheduler(std::function<Scene()>&& sceneMaker)
    : sceneMaker{ std::move(sceneMaker) } {}

void Scheduler::iterate(Controller& controller) {
    std::swap(prevScene, scene);

    auto stagesFuture = std::async(std::launch::async, [this, &controller] {
        std::ranges::for_each(stages, [&controller](auto& stage) {
            stage.run(controller);
        });
    });

    auto renderFuture =
        std::async(std::launch::async, Scene::render, std::ref(prevScene));

    scene = sceneMaker();

    // throw potential exception from threads
    stagesFuture.get();
    renderFuture.get();
}

void Scheduler::add_stage(Stage&& stage) {
    if (!Stage::empty(stage)) {
        stages.push_back(std::move(stage));
    }
}

[[nodiscard]] auto Scheduler::empty(Scheduler& scheduler) -> bool {
    return std::ranges::empty(scheduler.stages);
}
