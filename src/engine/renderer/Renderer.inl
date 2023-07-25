namespace engine {

///////////////////////////////////
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

auto Renderer::create(
    Renderer::Instance&&          t_instance,
    renderer::SurfaceCreator auto t_surface_creator
) noexcept -> std::optional<Renderer>
{
    std::optional<vk::SurfaceKHR> surface{
        t_surface_creator(*t_instance, nullptr)
    };
    if (!surface.has_value()) {
        return std::nullopt;
    }

    return create(
        std::move(t_instance), SwapChain::Surface{ *t_instance, *surface }
    );
}

}   // namespace engine
