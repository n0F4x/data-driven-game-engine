#include "engine/Stage.hpp"

#include <algorithm>
#include <future>
#include <vector>

#include "engine/Controller.hpp"

namespace engine {

void Stage::run(Controller& t_controller) const {
    std::vector<std::future<void>> futures;

    std::ranges::for_each(m_systems, [&futures, &t_controller](auto t_system) {
        futures.push_back(
            std::async(std::launch::async, t_system, std::ref(t_controller)));
    });

    // throw potential exception from threads
    std::ranges::for_each(futures, &std::future<void>::get);
}

}   // namespace engine
