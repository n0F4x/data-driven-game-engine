module;

#include <functional>

#include <VkBootstrap.h>

#include "core/log/log.hpp"

export module plugins.renderer.RendererPlugin;

import vulkan_hpp;

import core.app.Builder;

import core.gfx.resources.Image;
import core.gfx.resources.VirtualImage;

import core.renderer.base.instance.Instance;
import core.renderer.base.device.Device;
import core.renderer.base.allocator.Allocator;
import core.renderer.base.swapchain.Swapchain;
import core.renderer.base.swapchain.SwapchainHolder;
import core.renderer.model.ModelLayout;

import plugins.renderer.DevicePlugin;
import plugins.renderer.InstancePlugin;
import plugins.renderer.SurfacePlugin;

import extensions.ResourceManager;
import extensions.Functional;

namespace plugins {

namespace renderer {

export struct Requirement {
    std::function<bool(const vkb::SystemInfo&)> required_instance_settings_are_available;
    std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)>
                                                      enable_instance_settings;
    std::function<void(vkb::PhysicalDeviceSelector&)> require_device_settings;
    std::function<void(vkb::PhysicalDevice&)>         enable_optional_device_settings;
};

export template <extensions::injection_c SurfacePlugin_T>
class RendererPlugin {
public:
    RendererPlugin();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    template <
        core::app::extended_with_c<extensions::ResourceManagerTag, extensions::Functional>
            Builder_T>
    auto operator()(Builder_T&& builder);

    template <typename Self>
    auto require(this Self&&, Requirement requirement) -> Self;

    template <typename Self, typename... Args>
    auto set_framebuffer_size_getter(this Self&&, Args&&... args) -> Self;

private:
    std::vector<Requirement> m_requirements;
    SurfacePlugin_T          m_surface_plugin{};
};

}   // namespace renderer

export using Renderer = renderer::RendererPlugin<renderer::SurfacePlugin>;

}   // namespace plugins

template <typename Requirement_T>
auto make_requirement() -> plugins::renderer::Requirement
{
    plugins::renderer::Requirement requirement;

    if constexpr (requires(Requirement_T, const vkb::SystemInfo& system_info) {
                      {
                          Requirement_T::required_instance_settings_are_available(
                              system_info
                          )
                      } -> std::same_as<bool>;
                  })
    {
        requirement.required_instance_settings_are_available =
            static_cast<bool (*)(const vkb::SystemInfo& system_info)>(
                &Requirement_T::required_instance_settings_are_available
            );
    }

    if constexpr (requires(
                      const vkb::SystemInfo& system_info, vkb::InstanceBuilder& builder
                  ) {
                      {
                          Requirement_T::enable_instance_settings(system_info)
                      } -> std::same_as<void>;
                  })
    {
        requirement.enable_instance_settings =
            static_cast<void (*)(const vkb::SystemInfo&, vkb::InstanceBuilder&)>(
                &Requirement_T::enable_instance_settings
            );
    }

    if constexpr (requires(vkb::PhysicalDeviceSelector& physical_device_selector) {
                      {
                          Requirement_T::require_device_settings(physical_device_selector)
                      } -> std::same_as<void>;
                  })
    {
        requirement.require_device_settings =
            static_cast<void (*)(vkb::PhysicalDeviceSelector&)>(
                &Requirement_T::require_device_settings
            );
    }

    if constexpr (requires(vkb::PhysicalDevice& physical_device) {
                      {
                          Requirement_T::enable_optional_device_settings(physical_device)
                      } -> std::same_as<void>;
                  })
    {
        requirement.enable_optional_device_settings =
            static_cast<void (*)(vkb::PhysicalDevice&)>(
                &Requirement_T::enable_optional_device_settings
            );
    }

    return requirement;
}

template <extensions::injection_c SurfacePlugin_T>
plugins::renderer::RendererPlugin<SurfacePlugin_T>::RendererPlugin()
{
    require(::make_requirement<core::renderer::base::Allocator::Requirements>());
    require(::make_requirement<core::renderer::base::Swapchain::Requirements>());
    require(::make_requirement<core::renderer::ModelLayout::Requirements>());
    require(::make_requirement<core::gfx::resources::Image::Requirements>());
    require(::make_requirement<core::gfx::resources::VirtualImage::Requirements>());
}

[[nodiscard]]
auto to_instance_dependency(const plugins::renderer::Requirement& requirement)
    -> plugins::renderer::InstancePlugin::Dependency
{
    return plugins::renderer::InstancePlugin::Dependency{
        .required_settings_are_available =
            requirement.required_instance_settings_are_available,
        .enable_settings = requirement.enable_instance_settings,
    };
}

[[nodiscard]]
auto to_device_dependency(const plugins::renderer::Requirement& requirement)
    -> plugins::renderer::DevicePlugin::Dependency
{
    return plugins::renderer::DevicePlugin::Dependency{
        .require_settings         = requirement.require_device_settings,
        .enable_optional_settings = requirement.enable_optional_device_settings,
    };
}

template <extensions::injection_c SurfacePlugin_T>
template <core::app::extended_with_c<extensions::ResourceManagerTag, extensions::Functional>
              Builder_T>
auto plugins::renderer::RendererPlugin<SurfacePlugin_T>::operator()(Builder_T&& builder)
{
    return std::forward<Builder_T>(builder)
        .inject_resource([this] {
            plugins::renderer::InstancePlugin instance_plugin{};
            for (const Requirement& requirement : m_requirements) {
                instance_plugin.emplace_dependency(::to_instance_dependency(requirement));
            }
            return instance_plugin;
        }())
        .inject_resource(m_surface_plugin)
        .inject_resource([this] {
            plugins::renderer::DevicePlugin device_plugin{};
            for (const Requirement& requirement : m_requirements) {
                device_plugin.emplace_dependency(::to_device_dependency(requirement));
            }
            return device_plugin;
        }())
        // TODO: reallow using create_framebuffer_size_getter
        .inject_resource([](const vk::UniqueSurfaceKHR&         surface,
                            const core::renderer::base::Device& device) {
            return core::renderer::base::SwapchainHolder{ surface.get(), device, nullptr };
        })
        .inject_resource([](const core::renderer::base::Instance& instance,
                            const core::renderer::base::Device&   device) {
            return core::renderer::base::Allocator{ instance, device };
        })
        .transform([]<typename B_T>(B_T&& b) -> B_T {
            ENGINE_LOG_TRACE("Added Renderer plugin group");
            return std::forward<B_T>(b);
        });
}

template <extensions::injection_c SurfacePlugin_T>
template <typename Self>
auto plugins::renderer::RendererPlugin<SurfacePlugin_T>::require(
    this Self&& self,
    Requirement requirement
) -> Self
{
    self.m_requirements.push_back(std::move(requirement));
    return std::forward<Self>(self);
}

template <extensions::injection_c SurfacePlugin_T>
template <typename Self, typename... Args>
auto plugins::renderer::RendererPlugin<SurfacePlugin_T>::set_framebuffer_size_getter(
    this Self&& self,
    Args&&... args
) -> Self
{
    self.m_create_framebuffer_size_getter.operator=(std::forward<Args>(args)...);
    return std::forward<Self>(self);
}
