#pragma once

#include "SurfaceProvider.hpp"

namespace plugins::renderer {

using FramebufferSizeGetterCreator =
    std::function<core::renderer::SwapchainHolder::FramebufferSizeGetter(Store&)>;

template <SurfaceProviderConcept SurfaceProvider>
class Options {
public:
    explicit Options(
        uint32_t                       required_vulkan_version,
        SurfaceProvider&&              surface_provider,
        FramebufferSizeGetterCreator&& create_framebuffer_size_getter
    ) noexcept;

    auto require_vulkan_version(uint32_t major, uint32_t minor, uint32_t patch = 0) noexcept
        -> Options&;

    template <SurfaceProviderConcept NewSurfaceProvider>
    auto set_surface_provider(NewSurfaceProvider&& surface_provider
    ) -> Options<NewSurfaceProvider>;

    auto set_framebuffer_size_getter(FramebufferSizeGetterCreator framebuffer_size_callback
    ) -> Options&;

    [[nodiscard]]
    auto required_vulkan_version() const noexcept -> uint32_t;
    [[nodiscard]]
    auto surface_provider() const noexcept -> const SurfaceProvider&;
    [[nodiscard]]
    auto framebuffer_size_getter() const noexcept -> const FramebufferSizeGetterCreator&;

private:
    uint32_t                     m_required_vulkan_version;
    SurfaceProvider              m_surface_provider;
    FramebufferSizeGetterCreator m_create_framebuffer_size_getter;
};

}   // namespace plugins::renderer

#include "Options.inl"
