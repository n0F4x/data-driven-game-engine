#pragma once

#include <functional>

#include <vulkan/vulkan.hpp>

#include <app/App.hpp>

namespace plugins {

namespace renderer {

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
    template <typename Self>
    auto require_vulkan_version(
        this Self&&,
        uint32_t major,
        uint32_t minor,
        uint32_t patch = 0
    ) noexcept -> Self;

    template <typename Self, typename SurfacePlugin>
    auto set_surface_plugin(this Self&&, SurfacePlugin surface_plugin) -> Self;

    template <typename Self, typename... Args>
    auto set_framebuffer_size_getter(this Self&&, Args&&... args) -> Self;

private:
    uint32_t m_required_vulkan_version{ vk::ApiVersion10 };
    std::function<void(App::Builder& app_builder)> m_surface_plugin_provider;
};

}   // namespace renderer

using Renderer = renderer::RendererPlugin;

}   // namespace plugins

#include "Renderer.inl"
