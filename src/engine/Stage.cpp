#include "engine/Stage.hpp"

#include <vector>
#include <future>
#include <algorithm>
#include <iostream>

#include "engine/Controller.hpp"


void Stage::run(Controller& controller) const {
    std::vector<std::future<void>> futures;

    std::ranges::for_each(systems, [&futures, &controller](auto system) {
        futures.push_back(std::async(std::launch::async, system, std::ref(controller))); });

    std::ranges::for_each(futures, &std::future<void>::get);
}
