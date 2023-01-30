#include <algorithm>
#include <functional>
#include <future>

namespace engine {

template <class TController>
BasicSchedule<TController>::BasicSchedule(StageContainer&& t_stages) noexcept
    : m_stages{ std::move(t_stages) } {}

template <class TController>
void BasicSchedule<TController>::execute(Controller t_controller) {
    while (t_controller.running()) {
        advance(t_controller);

        t_controller.stateMachine().transition();
    }
}

template <class TController>
void BasicSchedule<TController>::advance(Controller t_controller) {
    std::swap(m_previousScene, m_scene);

    auto stagesFuture = std::async(std::launch::async, [this, &t_controller] {
        std::ranges::for_each(m_stages, [&t_controller](auto& t_stage) {
            t_stage.run(t_controller);
        });
    });

    auto renderFuture = std::async(
        std::launch::async, &fw::Scene::render, std::ref(m_previousScene));

    m_scene = t_controller.sceneGraph().make_scene();

    // throw potential exception from threads
    stagesFuture.get();
    renderFuture.get();
}

template <class TController>
BasicSchedule<TController>::Builder::operator Product() noexcept {
    return build();
}

template <class TController>
auto BasicSchedule<TController>::Builder::build() noexcept -> Product {
    return BasicSchedule{ std::move(m_stages) };
}

template <class TController>
auto BasicSchedule<TController>::Builder::add_stage(Stage&& t_stage)
    -> Builder& {
    m_stages.push_back(std::move(t_stage));
    return *this;
}

}   // namespace engine
