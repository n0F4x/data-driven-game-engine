namespace plugins::renderer {

template <typename... Args>
SurfacePlugin::SurfacePlugin(std::in_place_t, Args&&... args)
    : m_surface_provider{ std::forward<Args>(args)... }
{}

}   // namespace plugins::renderer
