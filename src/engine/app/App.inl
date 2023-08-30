namespace engine {

auto App::run(const Runner<App> auto& t_runner) noexcept -> void
{
    t_runner(*this);
}

}   // namespace engine
