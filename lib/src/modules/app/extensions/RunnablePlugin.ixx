module;

#include <concepts>

#include "modules/log/log_macros.hpp"

export module modules.app.extensions.RunnablePlugin;

import modules.app.decays_to_builder_c;
import modules.log;

namespace modules::app::extensions {

template <typename Runner_T, typename Builder_T, typename... Args_T>
concept runner_c = requires(Runner_T&& runner, Builder_T&& builder, Args_T&&... args) {
    std::invoke(
        std::forward<Runner_T>(runner),
        std::forward<Builder_T>(builder).build(),
        std::forward<Args_T>(args)...
    );
};

export class RunnablePlugin {
public:
    template <decays_to_builder_c Self_T, typename... Args_T, runner_c<Self_T, Args_T...> Runner_T>
    constexpr auto run(this Self_T&& self, Runner_T&& runner, Args_T&&... args) -> std::
        invoke_result_t<Runner_T&&, decltype(std::forward<Self_T>(self).build()), Args_T&&...>;
};

}   // namespace modules::app::extensions

template <
    modules::app::decays_to_builder_c Self_T,
    typename... Args_T,
    modules::app::extensions::runner_c<Self_T, Args_T...> Runner_T>
constexpr auto modules::app::extensions::RunnablePlugin::run(
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

            if !consteval {
                ENGINE_LOG_INFO("App is running");
            }

            return app;
        }(),
        std::forward<Args_T>(args)...
    );
}
