#include <algorithm>
#include <functional>
#include <future>

namespace app {

template <class ControllerType>
BasicSchedule<ControllerType>::BasicSchedule(StageContainer&& t_stages) noexcept
    : m_stages{ std::move(t_stages) } {}

template <class ControllerType>
void BasicSchedule<ControllerType>::execute(Controller t_controller) {
    while (t_controller.running()) {
        advance(t_controller);

        t_controller.stateMachine().transition();
    }
}

template <class ControllerType>
void BasicSchedule<ControllerType>::advance(Controller t_controller) {
    auto stagesFuture = std::async(std::launch::async, [this, &t_controller] {
        std::ranges::for_each(m_stages, [&t_controller](auto& t_stage) {
            t_stage.run(t_controller);
        });
    });

    // throw potential exception from threads
    stagesFuture.get();
}

template <class ControllerType>
BasicSchedule<ControllerType>::Builder::operator Product() noexcept {
    return build();
}

template <class ControllerType>
auto BasicSchedule<ControllerType>::Builder::build() noexcept -> Product {
    return BasicSchedule{ std::move(m_stages) };
}

template <class ControllerType>
auto BasicSchedule<ControllerType>::Builder::add_stage(Stage&& t_stage)
    -> Builder& {
    m_stages.push_back(std::move(t_stage));
    return *this;
}

}   // namespace app
