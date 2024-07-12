#pragma once

#include <functional>

#include <vulkan/vulkan_core.h>

#include "core/renderer/base/swapchain/SwapchainHolder.hpp"
#include "plugins/renderer/DefaultSurfaceProvider.hpp"
#include "plugins/renderer/SurfaceProvider.hpp"

#include "app.hpp"

namespace plugins {

namespace renderer {

using FramebufferSizeGetterCreator =
    std::function<core::renderer::SwapchainHolder::FramebufferSizeGetter(App&)>;

template <SurfaceProviderConcept SurfaceProvider = DefaultSurfaceProvider>
class BasicRenderer {
public:
    auto require_vulkan_version(uint32_t major, uint32_t minor, uint32_t patch = 0) noexcept
        -> BasicRenderer&;

    template <SurfaceProviderConcept NewSurfaceProvider>
    auto set_surface_provider(NewSurfaceProvider&& surface_provider
    ) -> BasicRenderer<NewSurfaceProvider>;

    auto set_framebuffer_size_getter(FramebufferSizeGetterCreator framebuffer_size_callback
    ) -> BasicRenderer&;

    [[nodiscard]]
    auto required_vulkan_version() const noexcept -> uint32_t;
    [[nodiscard]]
    auto surface_provider() const noexcept -> const SurfaceProvider&;
    [[nodiscard]]
    auto framebuffer_size_getter() const noexcept -> const FramebufferSizeGetterCreator&;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App&) const -> void;

private:
    uint32_t                     m_required_vulkan_version{ VK_API_VERSION_1_0 };
    SurfaceProvider              m_surface_provider;
    FramebufferSizeGetterCreator m_create_framebuffer_size_getter;
};

}   // namespace renderer

using Renderer = renderer::BasicRenderer<>;

}   // namespace plugins

#include "Renderer.inl"
