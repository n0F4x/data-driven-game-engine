#include <spdlog/spdlog.h>

template <typename Context, typename Self, typename... Args>
auto App::Builder::store(this Self&& self, Args&&... args) -> Self
{
    std::forward<Self>(self).m_store.template emplace<Context>(std::forward<Args>(args)...
    );
    return std::forward<Self>(self);
}

template <typename Plugin, typename Self>
auto App::Builder::add_plugin(this Self&& self, auto&&... args) -> Self
{
    return std::forward<Self>(self).add_plugin(
        Plugin{}, std::forward<decltype(args)>(args)...
    );
}

template <typename Self, typename... Args>
auto App::Builder::add_plugin(
    this Self&&                   self,
    PluginConcept<Args...> auto&& plugin,
    Args&&... args
) -> Self
{
    std::invoke(
        std::forward<decltype(plugin)>(plugin),
        self.m_store,
        std::forward<decltype(args)>(args)...
    );
    return std::forward<Self>(self);
}

template <typename... Args>
auto App::Builder::run(
    RunnerConcept<Args...> auto&& runner,
    Args&&... args
) && -> std::invoke_result_t<decltype(runner), App, Args...>
{
    SPDLOG_INFO("App is running");
    return std::invoke(
        std::forward<decltype(runner)>(runner),
        std::move(*this).build(),
        std::forward<Args>(args)...
    );
}
