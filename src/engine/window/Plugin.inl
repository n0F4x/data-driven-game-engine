namespace engine::window {

auto Plugin::operator()(
    Store&              t_store,
    uint16_t            t_width,
    uint16_t            t_height,
    const std::string&  t_title,
    std::invocable auto t_configure
) -> void
{
    std::invoke(t_configure);
    operator()(t_store, t_width, t_height, t_title);
}

}   // namespace engine::window
