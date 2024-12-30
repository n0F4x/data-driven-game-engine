module;

#include <functional>

#include <spdlog/spdlog.h>

#include <VkBootstrap.h>

export module addons.renderer.RendererPlugin;

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

import addons.renderer.DevicePlugin;
import addons.renderer.InstancePlugin;
import addons.renderer.SurfacePlugin;

import addons.store.Customization;
import addons.functional.Customization;

namespace addons {

namespace renderer {

export struct Requirement {
    std::function<bool(const vkb::SystemInfo&)> required_instance_settings_are_available;
    std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)>
                                                      enable_instance_settings;
    std::function<void(vkb::PhysicalDeviceSelector&)> require_device_settings;
    std::function<void(vkb::PhysicalDevice&)>         enable_optional_device_settings;
};

export template <addons::store::plugin_c SurfacePlugin_T>
class RendererPlugin {
public:
    RendererPlugin();

    ///-------------///
    ///  Operators  ///
    ///-------------///
    template <core::app::customization_of_c<
        addons::store::Customization,
        addons::functional::Customization> Builder_T>
    auto operator()(Builder_T&& builder) -> Builder_T;

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

}   // namespace addons

template <typename Requirement_T>
auto make_requirement() -> addons::renderer::Requirement
{
    addons::renderer::Requirement requirement;

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

template <addons::store::plugin_c SurfacePlugin_T>
addons::renderer::RendererPlugin<SurfacePlugin_T>::RendererPlugin()
{
    require(::make_requirement<core::renderer::base::Allocator::Requirements>());
    require(::make_requirement<core::renderer::base::Swapchain::Requirements>());
    require(::make_requirement<core::renderer::ModelLayout::Requirements>());
    require(::make_requirement<core::gfx::resources::Image::Requirements>());
    require(::make_requirement<core::gfx::resources::VirtualImage::Requirements>());
}

[[nodiscard]]
auto to_instance_dependency(const addons::renderer::Requirement& requirement)
    -> addons::renderer::InstancePlugin::Dependency
{
    return addons::renderer::InstancePlugin::Dependency{
        .required_settings_are_available =
            requirement.required_instance_settings_are_available,
        .enable_settings = requirement.enable_instance_settings,
    };
}

[[nodiscard]]
auto to_device_dependency(const addons::renderer::Requirement& requirement)
    -> addons::renderer::DevicePlugin::Dependency
{
    return addons::renderer::DevicePlugin::Dependency{
        .require_settings         = requirement.require_device_settings,
        .enable_optional_settings = requirement.enable_optional_device_settings,
    };
}

template <addons::store::plugin_c SurfacePlugin_T>
template <core::app::customization_of_c<
    addons::store::Customization,
    addons::functional::Customization> Builder_T>
auto addons::renderer::RendererPlugin<SurfacePlugin_T>::operator()(Builder_T&& builder)
    -> Builder_T
{
    return std::forward<Builder_T>(builder)
        .inject([this] {
            addons::renderer::InstancePlugin instance_plugin{};
            for (const Requirement& requirement : m_requirements) {
                instance_plugin.emplace_dependency(::to_instance_dependency(requirement));
            }
            return instance_plugin;
        }())
        .inject(m_surface_plugin)
        .inject([this] {
            addons::renderer::DevicePlugin device_plugin{};
            for (const Requirement& requirement : m_requirements) {
                device_plugin.emplace_dependency(::to_device_dependency(requirement));
            }
            return device_plugin;
        }())
        // TODO: reallow using create_framebuffer_size_getter
        .inject([](const vk::UniqueSurfaceKHR&         surface,
                   const core::renderer::base::Device& device) {
            return core::renderer::base::SwapchainHolder{ surface.get(), device, nullptr };
        })
        .inject([](const core::renderer::base::Instance& instance,
                   const core::renderer::base::Device&   device) {
            return core::renderer::base::Allocator{ instance, device };
        })
        .apply([](Builder_T&& b) -> Builder_T {
            SPDLOG_TRACE("Added Renderer plugin group");
            return std::forward<Builder_T>(b);
        });
}

template <addons::store::plugin_c SurfacePlugin_T>
template <typename Self>
auto addons::renderer::RendererPlugin<SurfacePlugin_T>::require(
    this Self&& self,
    Requirement requirement
) -> Self
{
    self.m_requirements.push_back(std::move(requirement));
    return std::forward<Self>(self);
}

template <addons::store::plugin_c SurfacePlugin_T>
template <typename Self, typename... Args>
auto addons::renderer::RendererPlugin<SurfacePlugin_T>::set_framebuffer_size_getter(
    this Self&& self,
    Args&&... args
) -> Self
{
    self.m_create_framebuffer_size_getter.operator=(std::forward<Args>(args)...);
    return std::forward<Self>(self);
}
