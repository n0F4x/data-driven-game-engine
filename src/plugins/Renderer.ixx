module;

#include <functional>

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

export module plugins.renderer.RendererPlugin;

import app.App;

namespace plugins {

namespace renderer {

export struct Requirement {
    std::function<bool(const vkb::SystemInfo&)> required_instance_settings_are_available;
    std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)>
                                                      enable_instance_settings;
    std::function<void(vkb::PhysicalDeviceSelector&)> require_device_settings;
    std::function<void(vkb::PhysicalDevice&)>         enable_optional_device_settings;
};

export class RendererPlugin {
public:
    RendererPlugin();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator()(App::Builder& app_builder) const -> void;

    template <typename Self>
    auto require(this Self&&, Requirement requirement) -> Self;

    template <typename Self, typename SurfacePlugin>
    auto set_surface_plugin(this Self&&, SurfacePlugin surface_plugin) -> Self;

    template <typename Self, typename... Args>
    auto set_framebuffer_size_getter(this Self&&, Args&&... args) -> Self;

private:
    std::vector<Requirement> m_requirements;
    std::function<void(App::Builder& app_builder)> m_surface_plugin_provider;
};

}   // namespace renderer

export using Renderer = renderer::RendererPlugin;

}   // namespace plugins

#include "Renderer.inl"
