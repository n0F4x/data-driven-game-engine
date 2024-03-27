namespace plugins {

auto Window::operator()(
    app::App::Builder&  t_builder,
    const uint16_t      t_width,
    const uint16_t      t_height,
    const std::string&  t_title,
    std::invocable auto t_configure
) const -> void
{
    std::invoke(t_configure);
    operator()(t_builder, t_width, t_height, t_title);
}

}   // namespace plugins
