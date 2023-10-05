namespace engine {

auto App::run(const RunnerConcept<App> auto& t_runner) noexcept -> void
{
    t_runner(*this);
}

}   // namespace engine
