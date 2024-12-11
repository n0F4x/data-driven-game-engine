#pragma once

template <typename Self>
auto plugins::renderer::RendererPlugin::require(this Self&& self, Requirement requirement)
    -> Self
{
    self.m_requirements.push_back(std::move(requirement));
    return std::forward<Self>(self);
}

template <typename Self, typename SurfacePlugin>
auto plugins::renderer::RendererPlugin::set_surface_plugin(
    this Self&&   self,
    SurfacePlugin surface_plugin
) -> Self
{
    self.m_surface_plugin_provider =
        [surface_plugin = std::move(surface_plugin)](App::Builder& app_builder) mutable {
            app_builder.use(std::move(surface_plugin));
        };
    return std::forward<Self>(self);
}

template <typename Self, typename... Args>
auto plugins::renderer::RendererPlugin::set_framebuffer_size_getter(
    this Self&& self,
    Args&&... args
) -> Self
{
    self.m_create_framebuffer_size_getter.operator=(std::forward<Args>(args)...);
    return std::forward<Self>(self);
}
