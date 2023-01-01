#include "engine/Scheduler.hpp"

#include <vector>
#include <future>
#include <algorithm>
#include <functional>
#include <ranges>

#include "engine/Stage.hpp"


void Scheduler::iterate(Controller& controller) {
    // TODO: copy from world to render-world

    std::vector<std::future<void>> futures;

    futures.push_back(std::async(std::launch::async, [this, &controller] {
        std::ranges::for_each(appStages, std::bind_back(&Stage::run, std::ref(controller))); }));
    futures.push_back(std::async(std::launch::async, [this, &controller] {
        std::ranges::for_each(renderStages, std::bind_back(&Stage::run, std::ref(controller))); }));

    // throw potential exception from threads
    std::ranges::for_each(futures, &std::future<void>::get);
}

void Scheduler::add_stage(Stage&& stage) {
    if (!Stage::empty(stage))
        appStages.push_back(std::move(stage));
}

void Scheduler::add_render_stage(Stage&& stage) {
    if (!Stage::empty(stage))
        renderStages.push_back(std::move(stage));
}


[[nodiscard]] auto Scheduler::empty(Scheduler& scheduler) -> bool
{
    return std::ranges::empty(scheduler.appStages) && std::ranges::empty(scheduler.renderStages);
}
