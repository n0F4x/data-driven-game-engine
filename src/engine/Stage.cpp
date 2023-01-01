#include "engine/Stage.hpp"

#include <vector>
#include <future>
#include <algorithm>

#include "engine/Controller.hpp"


void Stage::run(Controller& controller) const {
    std::vector<std::future<void>> futures;
    futures.resize(systems.size());

    std::ranges::for_each(systems, [&futures, &controller](auto system) {
        futures.push_back(std::async(std::launch::async, system, std::ref(controller))); });
}
