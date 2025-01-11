module;

#include <concepts>

#include <spdlog/spdlog.h>

export module addons.runnable.Customization;

import core.app.Builder;

namespace addons::runnable {

template <typename Runner_T, typename Builder_T, typename... Args_T>
concept runner_c = requires(Runner_T&& runner, Builder_T&& builder, Args_T&&... args) {
    std::invoke(
        std::forward<Runner_T>(runner),
        std::forward<Builder_T>(builder).build(),
        std::forward<Args_T>(args)...
    );
};

export class Customization {
public:
    template <
        core::app::builder_c Self_T,
        typename... Args_T,
        runner_c<Self_T, Args_T...> Runner_T>
    auto run(this Self_T&& self, Runner_T&& runner, Args_T&&... args) -> std::
        invoke_result_t<Runner_T&&, decltype(std::forward<Self_T>(self).build()), Args_T&&...>;
};

}   // namespace addons::runnable

template <
    core::app::builder_c Self_T,
    typename... Args_T,
    addons::runnable::runner_c<Self_T, Args_T...> Runner_T>
auto addons::runnable::Customization::run(
    this Self_T&& self,
    Runner_T&&    runner,
    Args_T&&... args
) -> std::
    invoke_result_t<Runner_T&&, decltype(std::forward<Self_T>(self).build()), Args_T&&...>
{
    return std::invoke(
        std::forward<Runner_T>(runner),
        [&] {
            auto app{ std::forward<Self_T>(self).build() };
            SPDLOG_INFO("App is running");
            return app;
        }(),
        std::forward<Args_T>(args)...
    );
}
