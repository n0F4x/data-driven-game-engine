#include <spdlog/spdlog.h>

template <typename... Args>
auto App::Builder::run(
    RunnerConcept<Args...> auto&& t_runner,
    Args&&... t_args
) && -> std::invoke_result_t<decltype(t_runner), App, Args...>
{
    SPDLOG_INFO("App is running");
    return std::invoke(
        std::forward<decltype(t_runner)>(t_runner),
        std::move(*this).build(),
        std::forward<Args>(t_args)...
    );
}

template <typename Plugin>
auto App::Builder::add_plugin(auto&&... t_args) && -> Builder
{
    return std::move(*this).add_plugin(
        Plugin{}, std::forward<decltype(t_args)>(t_args)...
    );
}

template <typename... Args>
auto App::Builder::add_plugin(
    PluginConcept<Args...> auto&& t_plugin,
    Args&&... t_args
) && -> Builder
{
    std::invoke(
        std::forward<decltype(t_plugin)>(t_plugin),
        *this,
        std::forward<decltype(t_args)>(t_args)...
    );
    return std::move(*this);
}
