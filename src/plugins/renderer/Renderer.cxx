module;

#include <../../../build/Clang-MinGW-debug/_deps/vk-bootstrap-src/src/VkBootstrap.h>
#include <../../../build/Clang-MinGW-debug/vcpkg_installed/x64-mingw-static/include/spdlog/spdlog.h>

module plugins.renderer.RendererPlugin;

import vulkan_hpp;

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

template <typename RequirementT>
static auto make_requirement() -> plugins::renderer::Requirement
{
    plugins::renderer::Requirement requirement;

    if constexpr (requires(RequirementT, const vkb::SystemInfo& system_info) {
                      {
                          RequirementT::required_instance_settings_are_available(
                              system_info
                          )
                      } -> std::same_as<bool>;
                  })
    {
        requirement.required_instance_settings_are_available =
            static_cast<bool (*)(const vkb::SystemInfo& system_info)>(
                &RequirementT::required_instance_settings_are_available
            );
    }

    if constexpr (requires(
                      const vkb::SystemInfo& system_info, vkb::InstanceBuilder& builder
                  ) {
                      {
                          RequirementT::enable_instance_settings(system_info)
                      } -> std::same_as<void>;
                  })
    {
        requirement.enable_instance_settings =
            static_cast<void (*)(const vkb::SystemInfo&, vkb::InstanceBuilder&)>(
                &RequirementT::enable_instance_settings
            );
    }

    if constexpr (requires(vkb::PhysicalDeviceSelector& physical_device_selector) {
                      {
                          RequirementT::require_device_settings(physical_device_selector)
                      } -> std::same_as<void>;
                  })
    {
        requirement.require_device_settings =
            static_cast<void (*)(vkb::PhysicalDeviceSelector&)>(
                &RequirementT::require_device_settings
            );
    }

    if constexpr (requires(vkb::PhysicalDevice& physical_device) {
                      {
                          RequirementT::enable_optional_device_settings(physical_device)
                      } -> std::same_as<void>;
                  })
    {
        requirement.enable_optional_device_settings =
            static_cast<void (*)(vkb::PhysicalDevice&)>(
                &RequirementT::enable_optional_device_settings
            );
    }

    return requirement;
}

plugins::renderer::RendererPlugin::RendererPlugin()
    : m_surface_plugin_provider{ [](core::app::Builder& app_builder) {
          app_builder.use(SurfacePlugin{});
      } }
{
    require(::make_requirement<core::renderer::base::Allocator::Requirements>());
    require(::make_requirement<core::renderer::base::Swapchain::Requirements>());
    require(::make_requirement<core::renderer::ModelLayout::Requirements>());
    require(::make_requirement<core::gfx::resources::Image::Requirements>());
    require(::make_requirement<core::gfx::resources::VirtualImage::Requirements>());
}

[[nodiscard]]
static auto to_instance_dependency(const plugins::renderer::Requirement& requirement)
    -> plugins::renderer::InstancePlugin::Dependency
{
    return plugins::renderer::InstancePlugin::Dependency{
        .required_settings_are_available =
            requirement.required_instance_settings_are_available,
        .enable_settings = requirement.enable_instance_settings,
    };
}

[[nodiscard]]
static auto to_device_dependency(const plugins::renderer::Requirement& requirement)
    -> plugins::renderer::DevicePlugin::Dependency
{
    return plugins::renderer::DevicePlugin::Dependency{
        .require_settings         = requirement.require_device_settings,
        .enable_optional_settings = requirement.enable_optional_device_settings,
    };
}

auto plugins::renderer::RendererPlugin::operator()(core::app::Builder& app_builder) const
    -> void
{
    app_builder.use([this] {
        plugins::renderer::InstancePlugin instance_plugin{};
        for (const Requirement& requirement : m_requirements) {
            instance_plugin.emplace_dependency(::to_instance_dependency(requirement));
        }
        return instance_plugin;
    }());

    m_surface_plugin_provider(app_builder);

    app_builder.use([this] {
        plugins::renderer::DevicePlugin device_plugin{};
        for (const Requirement& requirement : m_requirements) {
            device_plugin.emplace_dependency(::to_device_dependency(requirement));
        }
        return device_plugin;
    }());

    // TODO: reallow using create_framebuffer_size_getter
    app_builder.use([get_framebuffer_size = nullptr](
                        const vk::UniqueSurfaceKHR&         surface,
                        const core::renderer::base::Device& device
                    ) {
        return core::renderer::base::SwapchainHolder{ surface.get(),
                                                      device,
                                                      get_framebuffer_size };
    });

    app_builder.use([](const core::renderer::base::Instance& instance,
                       const core::renderer::base::Device&   device) {
        return core::renderer::base::Allocator{ instance, device };
    });

    SPDLOG_TRACE("Added Renderer plugin group");
}
