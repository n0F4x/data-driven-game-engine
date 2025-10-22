module;

#include <utility>

export module ddge.modules.exec.v2.gatherers.gatherer_of_c;

import ddge.modules.exec.v2.TaskHubProxy;

namespace ddge::exec::v2 {

template <typename Gatherer_T, typename Input_T>
inline constexpr bool can_receive_v = requires(Gatherer_T gatherer, Input_T&& input) {
    gatherer.receive(std::move(input));
};

template <typename Gatherer_T>
inline constexpr bool can_receive_v<Gatherer_T, void> = requires(Gatherer_T gatherer) {
    gatherer.receive();
};

export template <typename T, typename Input_T>
concept gatherer_of_c = can_receive_v<T, Input_T>
                     && requires(T gatherer, const TaskHubProxy& task_hub_proxy) {
                            gatherer.set_task_hub_proxy(task_hub_proxy);
                        };

}   // namespace ddge::exec::v2
