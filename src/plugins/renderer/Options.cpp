#include "plugins/Renderer.hpp"

namespace plugins {

auto Renderer::Options::require_vulkan_version(
    uint32_t t_major,
    uint32_t t_minor,
    uint32_t t_patch
) noexcept -> Options&
{
    m_required_vulkan_version = VK_MAKE_API_VERSION(0, t_major, t_minor, t_patch);
    return *this;
}

auto Renderer::Options::set_surface_creator(Renderer::SurfaceCreator t_surface_callback
) -> Options&
{
    m_create_surface = std::move(t_surface_callback);
    return *this;
}

auto Renderer::Options::set_framebuffer_size_getter(
    Renderer::FramebufferSizeGetterCreator t_framebuffer_size_callback
) -> Options&
{
    m_create_framebuffer_size_getter = std::move(t_framebuffer_size_callback);
    return *this;
}

[[nodiscard]]
auto Renderer::Options::required_vulkan_version() const noexcept -> uint32_t
{
    return m_required_vulkan_version;
}

[[nodiscard]]
auto Renderer::Options::surface_creator() const noexcept -> const Renderer::SurfaceCreator&
{
    return m_create_surface;
}

[[nodiscard]]
auto Renderer::Options::framebuffer_size_getter() const noexcept
    -> const Renderer::FramebufferSizeGetterCreator&
{
    return m_create_framebuffer_size_getter;
}

}   // namespace plugins
