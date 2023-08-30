namespace engine {

auto App::Builder::build_and_run(const Runner<App> auto& t_runner) && noexcept
    -> Result
{
    if (auto app{ std::move(*this).build() }) {
        app->run(t_runner);
        return Result::eSuccess;
    }
    return Result::eFailure;
}

auto App::Builder::add_plugin(const PluginConcept auto& t_plugin) && noexcept
    -> App::Builder
{
    t_plugin.set_context(m_context);
    return std::move(*this);
}

}   // namespace engine
