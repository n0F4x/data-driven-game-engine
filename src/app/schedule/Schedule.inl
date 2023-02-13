#include <algorithm>
#include <functional>
#include <future>

namespace app {

Schedule::Schedule(std::vector<Stage>&& t_stages) noexcept
    : m_stages{ std::move(t_stages) } {}

template<class AppView>
Schedule::operator std::function<void(AppView)>() {
    return [schedule = std::move(*this)] (auto app) mutable { schedule.execute(app); };
}

template<class AppView>
void Schedule::execute(AppView t_app) {
    std::cout << t_app.name() << " is running...\n";

    if (!m_stages.empty()) {
        Controller controller{ *this };
        while (m_running) {
            advance(t_app, controller);
        }
    }
}

void Schedule::quit() noexcept {
    m_running = false;
}

template<class AppView>
void Schedule::advance(AppView t_app, Controller t_controller) {
    for (auto& stage : m_stages) {
        stage.run(t_controller);
    }
    t_app.renderer().render();
}

Schedule::Builder::operator Product() noexcept {
    return build();
}

auto Schedule::Builder::build() noexcept -> Product {
    return Schedule{ std::move(m_stages) };
}

auto Schedule::Builder::add_stage(Stage&& t_stage) -> Builder& {
    m_stages.push_back(std::move(t_stage));
    return *this;
}

}   // namespace app
