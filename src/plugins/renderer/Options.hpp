#pragma once

namespace plugins {

class Renderer::Options {
    uint32_t                     m_required_vulkan_version{ VK_API_VERSION_1_0 };
    SurfaceCreator               m_create_surface{ create_default_surface };
    FramebufferSizeGetterCreator m_create_framebuffer_size_getter;

public:
    auto require_vulkan_version(uint32_t major, uint32_t minor, uint32_t patch = 0) noexcept
        -> Options&;
    auto set_surface_creator(SurfaceCreator surface_callback) -> Options&;
    auto set_framebuffer_size_getter(FramebufferSizeGetterCreator framebuffer_size_callback
    ) -> Options&;

    [[nodiscard]]
    auto required_vulkan_version() const noexcept -> uint32_t;
    [[nodiscard]]
    auto surface_creator() const noexcept -> const SurfaceCreator&;
    [[nodiscard]]
    auto framebuffer_size_getter() const noexcept -> const FramebufferSizeGetterCreator&;
};

}   // namespace plugins
