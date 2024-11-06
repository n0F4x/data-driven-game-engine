#include "Renderer.hpp"

#include <VkBootstrap.h>

#include "core/gfx/resources/Image.hpp"
#include "core/gfx/resources/VirtualImage.hpp"
#include "core/renderer/base/allocator/Requirements.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"
#include "core/renderer/base/swapchain/Requirements.hpp"
#include "core/renderer/model/Requirements.hpp"
#include "core/window/Window.hpp"
#include "plugins/renderer/DevicePlugin.hpp"
#include "plugins/renderer/InstancePlugin.hpp"
#include "plugins/renderer/SurfacePlugin.hpp"

plugins::renderer::RendererPlugin::RendererPlugin()
    : m_surface_plugin_provider{ [](App::Builder& app_builder) {
          app_builder.use(SurfacePlugin{});
      } }
{}

auto plugins::renderer::RendererPlugin::operator()(App::Builder& app_builder) const
    -> void
{
    app_builder.use(
        plugins::renderer::InstancePlugin{}
            .emplace_dependency(plugins::renderer::InstancePlugin::Dependency{
                .enable_instance_settings =
                    [vulkan_version = m_required_vulkan_version](
                        const vkb::SystemInfo&, vkb::InstanceBuilder& builder
                    ) { builder.require_api_version(vulkan_version); } })
            .emplace_dependency(
                core::renderer::base::Allocator::Requirements::
                    required_instance_settings_are_available,
                core::renderer::base::Allocator::Requirements::enable_instance_settings
            )
            .emplace_dependency(
                core::renderer::base::Swapchain::Requirements::
                    required_instance_settings_are_available,
                core::renderer::base::Swapchain::Requirements::enable_instance_settings
            )
    );

    m_surface_plugin_provider(app_builder);

    app_builder.use(
        plugins::renderer::DevicePlugin{}
            .emplace_dependency(
                core::renderer::base::Allocator::Requirements::require_device_settings,
                core::renderer::base::Allocator::Requirements::enable_optional_device_settings
            )
            .emplace_dependency(
                core::renderer::base::Swapchain::Requirements::require_device_settings
            )
            .emplace_dependency(
                core::renderer::ModelLayout::Requirements::require_device_settings
            )
            .emplace_dependency(
                core::gfx::resources::Image::Requirements::require_device_settings
            )
            .emplace_dependency(
                core::gfx::resources::VirtualImage::Requirements::require_device_settings
            )
    );

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
