#include "engine/Scheduler.hpp"

#include <vector>
#include <future>
#include <algorithm>
#include <functional>

#include "engine/Scene.hpp"
#include "engine/Stage.hpp"


Scheduler::Scheduler(SceneGraph& sceneGraph) : sceneGraph{ sceneGraph } {}


void Scheduler::iterate(Controller& controller) {
    std::swap(prevScene, scene);

    auto stagesFuture = std::async(std::launch::async, [this, &controller] {
        std::ranges::for_each(stages, std::bind_back(&Stage::run, std::ref(controller))); });

    auto renderFuture = std::async(std::launch::async, Scene::render, std::ref(prevScene));

    scene = sceneGraph.make_scene();

    // throw potential exception from threads
    stagesFuture.get();
    renderFuture.get();
}

void Scheduler::add_stage(Stage&& stage) {
    if (!Stage::empty(stage))
        stages.push_back(std::move(stage));
}


[[nodiscard]] auto Scheduler::empty(Scheduler& scheduler) -> bool
{
    return std::ranges::empty(scheduler.stages);
}
