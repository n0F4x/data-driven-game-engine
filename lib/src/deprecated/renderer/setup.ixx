module;

#include <functional>
#include <utility>

#include <VkBootstrap.h>

#include "modules/log/log_macros.hpp"

export module ddge.deprecated.renderer.setup;

import vulkan_hpp;

import ddge.modules.app.has_plugins_c;
import ddge.modules.app.extensions.FunctionalPlugin;

import ddge.deprecated.gfx.resources.Image;
import ddge.deprecated.gfx.resources.VirtualImage;

import ddge.modules.log;

import ddge.deprecated.renderer.base.instance.Instance;
import ddge.deprecated.renderer.base.device.Device;
import ddge.deprecated.renderer.base.allocator.Allocator;
import ddge.deprecated.renderer.base.swapchain.Swapchain;
import ddge.deprecated.renderer.base.swapchain.SwapchainHolder;
import ddge.deprecated.renderer.model.ModelLayout;

import ddge.deprecated.renderer.DeviceInjection;
import ddge.deprecated.renderer.InstanceInjection;
import ddge.deprecated.renderer.SurfaceInjection;

import ddge.modules.resources.Plugin;

namespace ddge::renderer {

export struct Requirement {
    std::function<bool(const vkb::SystemInfo&)> required_instance_settings_are_available;
    std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)>
                                                      enable_instance_settings;
    std::function<void(vkb::PhysicalDeviceSelector&)> require_device_settings;
    std::function<void(vkb::PhysicalDevice&)>         enable_optional_device_settings;
};

export template <resources::resource_injection_c SurfaceInjection_T>
class Setup {
public:
    Setup();

    template <ddge::app::has_plugins_c<resources::Plugin, app::extensions::FunctionalPlugin>
                  Builder_T>
    auto operator()(Builder_T&& builder) -> Builder_T;

    template <typename Self>
    auto require(this Self&&, Requirement requirement) -> Self;

private:
    std::vector<Requirement> m_requirements;
    SurfaceInjection_T       m_surface_injection{};
};

class SetupProxy {
public:
    template <ddge::app::has_plugins_c<resources::Plugin, app::extensions::FunctionalPlugin>
                  Builder_T>
    static auto operator()(Builder_T&& builder) -> Builder_T;

    static auto require(Requirement requirement) -> Setup<SurfaceInjection>;
};

export inline constexpr SetupProxy setup;

}   // namespace ddge::renderer

template <typename Requirement_T>
auto make_requirement() -> ddge::renderer::Requirement
{
    ddge::renderer::Requirement requirement;

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

template <ddge::resources::resource_injection_c SurfaceInjection_T>
ddge::renderer::Setup<SurfaceInjection_T>::Setup()
{
    require(::make_requirement<base::Allocator::Requirements>());
    require(::make_requirement<base::Swapchain::Requirements>());
    require(::make_requirement<ModelLayout::Requirements>());
    require(::make_requirement<gfx::resources::Image::Requirements>());
    require(::make_requirement<gfx::resources::VirtualImage::Requirements>());
}

[[nodiscard]]
auto to_instance_dependency(const ddge::renderer::Requirement& requirement)
    -> ddge::renderer::InstanceInjection::Dependency
{
    return ddge::renderer::InstanceInjection::Dependency{
        .required_settings_are_available =
            requirement.required_instance_settings_are_available,
        .enable_settings = requirement.enable_instance_settings,
    };
}

[[nodiscard]]
auto to_device_dependency(const ddge::renderer::Requirement& requirement)
    -> ddge::renderer::DeviceInjection::Dependency
{
    return ddge::renderer::DeviceInjection::Dependency{
        .require_settings         = requirement.require_device_settings,
        .enable_optional_settings = requirement.enable_optional_device_settings,
    };
}

template <ddge::resources::resource_injection_c SurfaceInjection_T>
template <ddge::app::has_plugins_c<
    ddge::resources::Plugin,
    ddge::app::extensions::FunctionalPlugin> Builder_T>
auto ddge::renderer::Setup<SurfaceInjection_T>::operator()(Builder_T&& builder)
    -> Builder_T
{
    return std::forward<Builder_T>(builder)
        .inject_resource([this] -> InstanceInjection {
            InstanceInjection instance_injection{};
            for (const Requirement& requirement : m_requirements) {
                instance_injection.emplace_dependency(
                    ::to_instance_dependency(requirement)
                );
            }
            return instance_injection;
        }())
        .inject_resource(m_surface_injection)
        .inject_resource([this] -> DeviceInjection {
            DeviceInjection device_injection{};
            for (const Requirement& requirement : m_requirements) {
                device_injection.emplace_dependency(::to_device_dependency(requirement));
            }
            return device_injection;
        }())
        // TODO: reallow using create_framebuffer_size_getter
        .inject_resource([](const vk::UniqueSurfaceKHR& surface,
                            const base::Device&         device) {
            return base::SwapchainHolder{ surface.get(), device, nullptr };
        })
        .inject_resource([](const base::Instance& instance, const base::Device& device) {
            return base::Allocator{ instance, device };
        })
        .transform([]<typename B_T>(B_T&& b) {
            ENGINE_LOG_TRACE("Added Renderer plugin group");
            return std::forward<B_T>(b);
        });
}

template <ddge::resources::resource_injection_c SurfaceInjection_T>
template <typename Self>
auto ddge::renderer::Setup<SurfaceInjection_T>::require(
    this Self&& self,
    Requirement requirement
) -> Self
{
    self.m_requirements.push_back(std::move(requirement));
    return std::forward<Self>(self);
}

template <ddge::app::has_plugins_c<
    ddge::resources::Plugin,
    ddge::app::extensions::FunctionalPlugin> Builder_T>
auto ddge::renderer::SetupProxy::operator()(Builder_T&& builder) -> Builder_T
{
    return std::forward<Builder_T>(builder).transform(Setup<SurfaceInjection>{});
}

module :private;

auto ddge::renderer::SetupProxy::require(Requirement requirement)
    -> Setup<SurfaceInjection>
{
    return Setup<SurfaceInjection>{}.require(std::move(requirement));
}
