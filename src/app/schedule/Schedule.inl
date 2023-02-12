#include <algorithm>
#include <functional>
#include <future>

namespace app {

template <class AppViewType>
Schedule<AppViewType>::Schedule(StageContainer&& t_stages) noexcept
    : m_stages{ std::move(t_stages) } {}

template <class AppViewType>
void Schedule<AppViewType>::execute(AppView t_app) {
    std::cout << t_app.name() << " is running...\n";

    if (!m_stages.empty()) {
        Controller controller{ *this };
        while (m_running) {
            advance(t_app, controller);
        }
    }
}

template <class AppViewType>
auto Schedule<AppViewType>::running() const noexcept -> bool {
    return m_running;
}

template <class AppViewType>
void Schedule<AppViewType>::quit() noexcept {
    m_running = false;
}

template <class AppViewType>
void Schedule<AppViewType>::advance(AppView t_app, Controller t_controller) {
    for (auto& stage : m_stages) {
        stage.run(t_controller);
    }
    t_app.renderer().render();
}

template <class AppViewType>
Schedule<AppViewType>::Builder::operator Product() noexcept {
    return build();
}

template <class AppViewType>
auto Schedule<AppViewType>::Builder::build() noexcept -> Product {
    return Schedule{ std::move(m_stages) };
}

template <class AppViewType>
auto Schedule<AppViewType>::Builder::add_stage(Stage&& t_stage) -> Builder& {
    m_stages.push_back(std::move(t_stage));
    return *this;
}

}   // namespace app
