namespace engine {

auto App::run(Runner auto t_runner) noexcept -> void {
    t_runner(m_renderer, m_window);
}

auto App::Builder::build_and_run(Runner auto t_runner) && noexcept -> Result
{
    if (auto app{std::move(*this).build()}) {
        app->run(t_runner);
        return Result::eSuccess;
    }
    return Result::eFailure;
}

}
