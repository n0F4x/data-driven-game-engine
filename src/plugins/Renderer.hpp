#pragma once

#include <functional>

#include "core/renderer/base/swapchain/SwapchainHolder.hpp"
#include "plugins/renderer/SurfacePlugin.hpp"

#include "app.hpp"

namespace plugins {

namespace renderer {

using FramebufferSizeGetterCreator =
    std::function<core::renderer::SwapchainHolder::FramebufferSizeGetter(App&)>;

class RendererPlugin {
public:
    RendererPlugin();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Builder& app_builder) const -> void;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Self, typename... Args>
    auto require_vulkan_version(
        this Self&&,
        uint32_t major,
        uint32_t minor,
        uint32_t patch = 0
    ) noexcept -> Self;

    template <typename Self, typename... Args>
    auto set_surface_plugin(this Self&&, Args&&... args) -> Self;

    template <typename Self, typename... Args>
    auto set_framebuffer_size_getter(this Self&&, Args&&... args) -> Self;

    [[nodiscard]]
    auto required_vulkan_version() const noexcept -> uint32_t;

    [[nodiscard]]
    auto surface_plugin() const noexcept -> const SurfacePlugin&;
    [[nodiscard]]
    auto surface_plugin() noexcept -> SurfacePlugin&;

    [[nodiscard]]
    auto framebuffer_size_getter() const noexcept -> const FramebufferSizeGetterCreator&;

private:
    uint32_t                     m_required_vulkan_version{ VK_API_VERSION_1_0 };
    SurfacePlugin                m_surface_plugin;
    FramebufferSizeGetterCreator m_create_framebuffer_size_getter;
};

}   // namespace renderer

using Renderer = renderer::RendererPlugin;

}   // namespace plugins

#include "Renderer.inl"
