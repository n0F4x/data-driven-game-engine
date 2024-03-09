namespace app {

template <typename... Args>
auto App::Builder::build_and_run(
    RunnerConcept<Args...> auto&& t_runner,
    Args&&... t_args
) && -> decltype(std::declval<App>()
                     .run(
                         std::forward<decltype(t_runner)>(t_runner),
                         std::forward<decltype(t_args)>(t_args)...
                     ))
{
    return std::move(*this).build().run(
        std::forward<decltype(t_runner)>(t_runner),
        std::forward<decltype(t_args)>(t_args)...
    );
}

template <typename Plugin>
auto App::Builder::add_plugin(auto&&... t_args) && -> App::Builder
{
    return std::move(*this).add_plugin(
        Plugin{}, std::forward<decltype(t_args)>(t_args)...
    );
}

template <typename... Args>
auto App::Builder::add_plugin(
    PluginConcept<Args...> auto&& t_plugin,
    Args&&... t_args
) && -> App::Builder
{
    std::invoke(
        std::forward<decltype(t_plugin)>(t_plugin),
        *this,
        std::forward<decltype(t_args)>(t_args)...
    );
    return std::move(*this);
}

}   // namespace engine
