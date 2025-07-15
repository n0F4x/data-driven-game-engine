module;

#include <functional>
#include <utility>

#include <VkBootstrap.h>

#include "core/log/log_macros.hpp"

export module extensions.renderer.setup;

import vulkan_hpp;

import app.has_plugins_c;

import core.gfx.resources.Image;
import core.gfx.resources.VirtualImage;

import core.log;

import core.renderer.base.instance.Instance;
import core.renderer.base.device.Device;
import core.renderer.base.allocator.Allocator;
import core.renderer.base.swapchain.Swapchain;
import core.renderer.base.swapchain.SwapchainHolder;
import core.renderer.model.ModelLayout;

import extensions.renderer.DeviceInjection;
import extensions.renderer.InstanceInjection;
import extensions.renderer.SurfaceInjection;

import plugins.resources;
import plugins.functional;

namespace extensions::renderer {

export struct Requirement {
    std::function<bool(const vkb::SystemInfo&)> required_instance_settings_are_available;
    std::function<void(const vkb::SystemInfo&, vkb::InstanceBuilder&)>
                                                      enable_instance_settings;
    std::function<void(vkb::PhysicalDeviceSelector&)> require_device_settings;
    std::function<void(vkb::PhysicalDevice&)>         enable_optional_device_settings;
};

export template <plugins::resource_injection_c SurfaceInjection_T>
class Setup {
public:
    Setup();

    template <app::has_plugins_c<plugins::ResourcesTag, plugins::Functional> Builder_T>
    auto operator()(Builder_T&& builder);

    template <typename Self>
    auto require(this Self&&, Requirement requirement) -> Self;

private:
    std::vector<Requirement> m_requirements;
    SurfaceInjection_T          m_surface_injection{};
};

class SetupProxy {
public:
    template <app::has_plugins_c<plugins::ResourcesTag, plugins::Functional> Builder_T>
    static auto operator()(Builder_T&& builder);

    static auto require(Requirement requirement) -> Setup<SurfaceInjection>;
};

export inline constexpr SetupProxy setup;

}   // namespace extensions::renderer

template <typename Requirement_T>
auto make_requirement() -> extensions::renderer::Requirement
{
    extensions::renderer::Requirement requirement;

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

template <plugins::resource_injection_c SurfaceInjection_T>
extensions::renderer::Setup<SurfaceInjection_T>::Setup()
{
    require(::make_requirement<core::renderer::base::Allocator::Requirements>());
    require(::make_requirement<core::renderer::base::Swapchain::Requirements>());
    require(::make_requirement<core::renderer::ModelLayout::Requirements>());
    require(::make_requirement<core::gfx::resources::Image::Requirements>());
    require(::make_requirement<core::gfx::resources::VirtualImage::Requirements>());
}

[[nodiscard]]
auto to_instance_dependency(const extensions::renderer::Requirement& requirement)
    -> extensions::renderer::InstanceInjection::Dependency
{
    return extensions::renderer::InstanceInjection::Dependency{
        .required_settings_are_available =
            requirement.required_instance_settings_are_available,
        .enable_settings = requirement.enable_instance_settings,
    };
}

[[nodiscard]]
auto to_device_dependency(const extensions::renderer::Requirement& requirement)
    -> extensions::renderer::DeviceInjection::Dependency
{
    return extensions::renderer::DeviceInjection::Dependency{
        .require_settings         = requirement.require_device_settings,
        .enable_optional_settings = requirement.enable_optional_device_settings,
    };
}

template <plugins::resource_injection_c SurfaceInjection_T>
template <app::has_plugins_c<plugins::ResourcesTag, plugins::Functional> Builder_T>
auto extensions::renderer::Setup<SurfaceInjection_T>::operator()(Builder_T&& builder)
{
    return std::forward<Builder_T>(builder)
        .inject_resource([this] {
            extensions::renderer::InstanceInjection instance_injection{};
            for (const Requirement& requirement : m_requirements) {
                instance_injection.emplace_dependency(::to_instance_dependency(requirement));
            }
            return instance_injection;
        }())
        .inject_resource(m_surface_injection)
        .inject_resource([this] {
            extensions::renderer::DeviceInjection device_injection{};
            for (const Requirement& requirement : m_requirements) {
                device_injection.emplace_dependency(::to_device_dependency(requirement));
            }
            return device_injection;
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
        .transform([]<typename B_T>(B_T&& b) {
            ENGINE_LOG_TRACE("Added Renderer plugin group");
            return std::forward<B_T>(b);
        });
}

template <plugins::resource_injection_c SurfaceInjection_T>
template <typename Self>
auto extensions::renderer::Setup<SurfaceInjection_T>::require(
    this Self&& self,
    Requirement requirement
) -> Self
{
    self.m_requirements.push_back(std::move(requirement));
    return std::forward<Self>(self);
}

template <app::has_plugins_c<plugins::ResourcesTag, plugins::Functional> Builder_T>
auto extensions::renderer::SetupProxy::operator()(Builder_T&& builder)
{
    return std::forward<Builder_T>(builder).transform(Setup<SurfaceInjection>{});
}

module :private;

auto extensions::renderer::SetupProxy::require(Requirement requirement)
    -> Setup<SurfaceInjection>
{
    return Setup<SurfaceInjection>{}.require(std::move(requirement));
}
