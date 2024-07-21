#include "Renderer.hpp"

#include <spdlog/spdlog.h>

#include <VkBootstrap.h>

#include "core/config/vulkan.hpp"
#include "core/renderer/base/allocator/Requirements.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"
#include "core/renderer/base/swapchain/Requirements.hpp"
#include "core/renderer/model/Requirements.hpp"
#include "core/window/Window.hpp"
#include "plugins/renderer/DevicePlugin.hpp"
#include "plugins/renderer/InstancePlugin.hpp"
#include "plugins/renderer/SurfacePlugin.hpp"

[[nodiscard]]
static auto create_surface(
    const App&                   app,
    const VkInstance             instance,
    const VkAllocationCallbacks* allocation_callbacks
) -> std::optional<VkSurfaceKHR>
{
    return app.resources.find<core::window::Window>().and_then(
        [instance, allocation_callbacks](const core::window::Window& window) {
            return window.create_vulkan_surface(instance, allocation_callbacks)
                .transform([](const VkSurfaceKHR surface) {
                    return std::make_optional(surface);
                })
                .value_or(std::nullopt);
        }
    );
}

static auto required_instance_settings_are_available(const vkb::SystemInfo& t_system_info
) -> bool
{
    return std::ranges::all_of(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::SystemInfo::is_extension_available, t_system_info)
    );
}

static auto enable_instance_settings(
    const vkb::SystemInfo&,
    vkb::InstanceBuilder& t_builder
) -> void
{
    std::ranges::for_each(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::InstanceBuilder::enable_extension, t_builder)
    );
}

static auto log_renderer_setup(const vkb::Device& t_device) -> void
{
    try {
        const auto instance_version{ t_device.instance_version };

        SPDLOG_INFO(
            "Created Vulkan Instance with version: {}.{}.{}",
            VK_VERSION_MAJOR(instance_version),
            VK_VERSION_MINOR(instance_version),
            VK_VERSION_PATCH(instance_version)
        );

        const auto properties{
            vk::PhysicalDevice(t_device.physical_device.physical_device).getProperties()
        };

        SPDLOG_INFO(
            "Chose GPU({}) with Vulkan version: {}.{}.{}",
            t_device.physical_device.name,
            VK_VERSION_MAJOR(properties.apiVersion),
            VK_VERSION_MINOR(properties.apiVersion),
            VK_VERSION_PATCH(properties.apiVersion)
        );

        std::string enabled_extensions{ "Enabled device extensions:" };
        for (const auto& extension : t_device.physical_device.get_extensions()) {
            enabled_extensions += '\n';
            enabled_extensions += '\t';
            enabled_extensions += extension;
        }
        SPDLOG_DEBUG(enabled_extensions);
    } catch (const vk::Error& t_error) {
        SPDLOG_ERROR(t_error.what());
    }
}

namespace plugins::renderer {

RendererPlugin::RendererPlugin()
    : m_surface_plugin{
          std::in_place,
          create_surface,
          renderer::InstancePlugin::Dependency{
                                               .required_instance_settings_are_available =
                  ::required_instance_settings_are_available,
                                               .enable_instance_settings = ::enable_instance_settings }
}
{}

auto RendererPlugin::required_vulkan_version() const noexcept -> uint32_t
{
    return m_required_vulkan_version;
}

auto RendererPlugin::surface_plugin() const noexcept -> const SurfacePlugin&
{
    return m_surface_plugin;
}

auto RendererPlugin::surface_plugin() noexcept -> SurfacePlugin&
{
    return m_surface_plugin;
}

auto RendererPlugin::framebuffer_size_getter() const noexcept
    -> const FramebufferSizeGetterCreator&
{
    return m_create_framebuffer_size_getter;
}

auto RendererPlugin::operator()(App::Builder& app_builder) const -> void
{
    app_builder.append(
        InstancePlugin{}
            .emplace_dependency(InstancePlugin::Dependency{
                .enable_instance_settings =
                    [vulkan_version = m_required_vulkan_version](
                        const vkb::SystemInfo&, vkb::InstanceBuilder& builder
                    ) { builder.require_api_version(vulkan_version); } })
            .emplace_dependency(
                core::renderer::Allocator::Requirements::
                    required_instance_settings_are_available,
                core::renderer::Allocator::Requirements::enable_instance_settings
            )
            .emplace_dependency(
                core::renderer::Swapchain::Requirements::
                    required_instance_settings_are_available,
                core::renderer::Swapchain::Requirements::enable_instance_settings
            )
    );

    app_builder.append<SurfacePlugin>(m_surface_plugin);

    app_builder.append(
        DevicePlugin{}
            .emplace_dependency(
                core::renderer::Allocator::Requirements::require_device_settings,
                core::renderer::Allocator::Requirements::enable_optional_device_settings
            )
            .emplace_dependency(
                core::renderer::Swapchain::Requirements::require_device_settings
            )
            .emplace_dependency(
                core::renderer::RenderModel::Requirements::require_device_settings
            )
    );

    app_builder.append(
        [create_framebuffer_size_getter = m_create_framebuffer_size_getter](App& app) {
            const std::optional surface{ app.resources.find<vk::UniqueSurfaceKHR>() };
            if (!surface.has_value()) {
                throw std::runtime_error{
                    "vk::UniqueSurfaceKHR required but not found in 'app.resources'"
                };
            }

            const std::optional device{ app.resources.find<core::renderer::Device>() };
            if (!device.has_value()) {
                throw std::runtime_error{
                    "Device required but not found in "
                    "'app.resources'"
                };
            }

            app.resources.emplace<core::renderer::SwapchainHolder>(
                surface.value().get().get(),
                device.value().get(),
                create_framebuffer_size_getter
                    ? std::invoke(create_framebuffer_size_getter, app)
                    : nullptr
            );
        }
    );

    app_builder.append([](App& app) {
        const std::optional instance{ app.resources.find<core::renderer::Instance>() };
        if (!instance.has_value()) {
            throw std::runtime_error{
                "core::renderer::Instance required but not found in 'app.resources'"
            };
        }

        const std::optional device{ app.resources.find<core::renderer::Device>() };
        if (!device.has_value()) {
            throw std::runtime_error{
                "core::renderer::Device required but not found in "
                "'app.resources'"
            };
        }

        app.resources.emplace<core::renderer::Allocator>(instance.value(), device.value());
    });

    app_builder.append([](App& app) {
        const std::optional device{ app.resources.find<core::renderer::Device>() };
        if (!device.has_value()) {
            throw std::runtime_error{
                "core::renderer::Device required but not found in "
                "'app.resources'"
            };
        }

        log_renderer_setup(static_cast<const vkb::Device>(device.value().get()));
    });

    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace plugins::renderer
