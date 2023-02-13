#include <algorithm>
#include <functional>
#include <future>

namespace app {

template <class AppType>
Schedule<AppType>::Schedule(StageContainer&& t_stages) noexcept
    : m_stages{ std::move(t_stages) } {}

template <class AppType>
Schedule<AppType>::operator std::function<void(typename AppType::AppView)>() {
    return [schedule = std::move(*this)] (auto app) mutable { schedule.execute(app); };
}

template <class AppType>
void Schedule<AppType>::execute(AppView t_app) {
    std::cout << t_app.name() << " is running...\n";

    if (!m_stages.empty()) {
        Controller controller{ *this };
        while (m_running) {
            advance(t_app, controller);
        }
    }
}

template <class AppType>
auto Schedule<AppType>::running() const noexcept -> bool {
    return m_running;
}

template <class AppType>
void Schedule<AppType>::quit() noexcept {
    m_running = false;
}

template <class AppType>
void Schedule<AppType>::advance(AppView t_app, Controller t_controller) {
    for (auto& stage : m_stages) {
        stage.run(t_controller);
    }
    t_app.renderer().render();
}

template <class AppType>
Schedule<AppType>::Builder::operator Product() noexcept {
    return build();
}

template <class AppType>
auto Schedule<AppType>::Builder::build() noexcept -> Product {
    return Schedule{ std::move(m_stages) };
}

template <class AppType>
auto Schedule<AppType>::Builder::add_stage(Stage&& t_stage) -> Builder& {
    m_stages.push_back(std::move(t_stage));
    return *this;
}

}   // namespace app
