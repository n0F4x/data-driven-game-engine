namespace plugins::renderer {

template <typename Self>
auto RendererPlugin::require_vulkan_version(
    this Self&&    self,
    const uint32_t t_major,
    const uint32_t t_minor,
    const uint32_t t_patch
) noexcept -> Self
{
    self.m_required_vulkan_version = vk::makeApiVersion(0u, t_major, t_minor, t_patch);
    return std::forward<Self>(self);
}

template <typename Self, typename... Args>
auto RendererPlugin::set_surface_plugin(this Self&& self, Args&&... args) -> Self
{
    self.m_surface_provider.operator=(std::forward<Args>(args)...);
    return std::forward<Self>(self);
}

template <typename Self, typename... Args>
auto RendererPlugin::set_framebuffer_size_getter(this Self&& self, Args&&... args) -> Self
{
    self.m_create_framebuffer_size_getter.operator=(std::forward<Args>(args)...);
    return std::forward<Self>(self);
}

}   // namespace plugins::renderer
