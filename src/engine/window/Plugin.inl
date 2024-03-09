namespace engine::window {

auto Plugin::operator()(
    App::Builder& t_builder,
    uint16_t            t_width,
    uint16_t            t_height,
    const std::string&  t_title,
    std::invocable auto t_configure
) -> void
{
    std::invoke(t_configure);
    operator()(t_builder, t_width, t_height, t_title);
}

}   // namespace engine::window
