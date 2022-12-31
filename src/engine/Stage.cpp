#include "engine/Stage.hpp"

#include <future>

#include "engine/Controller.hpp"


void Stage::run(Controller& controller) const {
    std::vector<std::future<void>> futures;
    futures.resize(systems.size());

    for (auto& system : systems)
        futures.push_back(std::async(std::launch::async, system, std::ref(controller)));
}
