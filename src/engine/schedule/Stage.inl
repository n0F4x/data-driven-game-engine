#include <future>
#include <ranges>

namespace engine {

Stage::Stage(std::vector<System>&& t_systems) noexcept
    : m_systems{ std::move(t_systems) } {}

void Stage::run(Controller t_controller) const {
    std::vector<std::future<void>> futures;

    for (const auto& system : m_systems) {
        futures.push_back(
            std::async(std::launch::async, system, std::ref(t_controller)));
    }

    // throw potential exception from threads
    for (auto& future : futures) {
        future.get();
    }
}

Stage::Builder::operator Product() noexcept {
    return build();
}

auto Stage::Builder::build() noexcept -> Product {
    return Stage{ std::move(m_systems) };
}

auto Stage::Builder::add_system(System&& t_system) -> Builder& {
    m_systems.push_back(std::move(t_system));
    return *this;
}

}   // namespace engine
