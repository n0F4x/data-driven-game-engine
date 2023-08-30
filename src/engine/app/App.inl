namespace engine {

auto App::run(Runner<App> auto t_runner) noexcept -> void
{
    t_runner(*this);
}

}   // namespace engine
