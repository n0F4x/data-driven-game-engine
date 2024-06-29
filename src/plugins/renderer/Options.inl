namespace plugins::renderer {

template <SurfaceProviderConcept SurfaceProvider>
Options<SurfaceProvider>::Options(
    uint32_t                       required_vulkan_version,
    SurfaceProvider&&              surface_provider,
    FramebufferSizeGetterCreator&& create_framebuffer_size_getter
) noexcept
    : m_required_vulkan_version{ required_vulkan_version },
      m_surface_provider{ std::move(surface_provider) },
      m_create_framebuffer_size_getter{ std::move(create_framebuffer_size_getter) }
{}

template <SurfaceProviderConcept SurfaceProvider>
auto Options<SurfaceProvider>::require_vulkan_version(
    const uint32_t t_major,
    const uint32_t t_minor,
    const uint32_t t_patch
) noexcept -> Options&
{
    m_required_vulkan_version = VK_MAKE_API_VERSION(0, t_major, t_minor, t_patch);
    return *this;
}

template <SurfaceProviderConcept SurfaceProvider>
template <SurfaceProviderConcept NewSurfaceProvider>
auto Options<SurfaceProvider>::set_surface_provider(NewSurfaceProvider&& t_surface_provider
) -> Options<NewSurfaceProvider>
{
    return Options<NewSurfaceProvider>{ m_required_vulkan_version,
                                        std::move(t_surface_provider),
                                        std::move(m_create_framebuffer_size_getter) };
}

template <SurfaceProviderConcept SurfaceProvider>
auto Options<SurfaceProvider>::set_framebuffer_size_getter(
    FramebufferSizeGetterCreator t_framebuffer_size_callback
) -> Options&
{
    m_create_framebuffer_size_getter = std::move(t_framebuffer_size_callback);
    return *this;
}

template <SurfaceProviderConcept SurfaceProvider>
auto Options<SurfaceProvider>::required_vulkan_version() const noexcept -> uint32_t
{
    return m_required_vulkan_version;
}

template <SurfaceProviderConcept SurfaceProvider>
auto Options<SurfaceProvider>::surface_provider() const noexcept -> const SurfaceProvider&
{
    return m_surface_provider;
}

template <SurfaceProviderConcept SurfaceProvider>
auto Options<SurfaceProvider>::framebuffer_size_getter() const noexcept
    -> const FramebufferSizeGetterCreator&
{
    return m_create_framebuffer_size_getter;
}

}   // namespace plugins::renderer
