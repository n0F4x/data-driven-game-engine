namespace plugins::renderer {

template <typename Self>
auto RendererPlugin::require_vulkan_version(
    this Self&&    self,
    const uint32_t major,
    const uint32_t minor,
    const uint32_t patch
) noexcept -> Self
{
    self.m_required_vulkan_version = vk::makeApiVersion(0u, major, minor, patch);
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
