#include "engine/Stage.hpp"

#include <algorithm>
#include <future>
#include <vector>

#include "engine/Controller.hpp"

namespace engine {

[[nodiscard]] auto Stage::empty() const noexcept -> bool {
    return std::ranges::empty(m_systems);
}

void Stage::run(Controller& t_controller) const {
    std::vector<std::future<void>> futures;

    std::ranges::for_each(m_systems, [&futures, &t_controller](auto t_system) {
        futures.push_back(
            std::async(std::launch::async, t_system, std::ref(t_controller)));
    });

    // throw potential exception from threads
    std::ranges::for_each(futures, &std::future<void>::get);
}

[[nodiscard]] auto Stage::create() noexcept -> Builder {
    return Builder{};
}

[[nodiscard]] auto Stage::Builder::add_system(System&& t_system) -> Builder& {
    draft().m_systems.push_back(std::move(t_system));

    return *this;
}

}   // namespace engine
