module;

#include <utility>

export module ddge.modules.scheduler.gatherers.gatherer_of_c;

import ddge.modules.scheduler.TaskContinuation;

namespace ddge::scheduler {

template <typename Gatherer_T, typename Input_T>
inline constexpr bool can_receive_v = requires(Gatherer_T gatherer, Input_T&& input) {
    gatherer.receive(std::move(input));
};

template <typename Gatherer_T>
inline constexpr bool can_receive_v<Gatherer_T, void> = requires(Gatherer_T gatherer) {
    gatherer.receive();
};

export template <typename T, typename Input_T>
concept gatherer_of_c = can_receive_v<T, Input_T> && requires {
    typename T::Output;
} && requires(T gatherer, TaskContinuation<typename T::Output>&& continuation) {
    gatherer.set_continuation(std::move(continuation));
};

}   // namespace ddge::scheduler
