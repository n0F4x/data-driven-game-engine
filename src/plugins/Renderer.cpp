#include "Renderer.hpp"

#include <VkBootstrap.h>

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

static auto required_instance_settings_are_available(const vkb::SystemInfo& system_info
) -> bool
{
    return std::ranges::all_of(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::SystemInfo::is_extension_available, system_info)
    );
}

static auto
    enable_instance_settings(const vkb::SystemInfo&, vkb::InstanceBuilder& builder) -> void
{
    std::ranges::for_each(
        core::window::Window::vulkan_instance_extensions(),
        std::bind_front(&vkb::InstanceBuilder::enable_extension, builder)
    );
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
    app_builder.use(
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

    app_builder.use<SurfacePlugin>(m_surface_plugin);

    app_builder.use(
        DevicePlugin{}
            .emplace_dependency(
                core::renderer::Allocator::Requirements::require_device_settings,
                core::renderer::Allocator::Requirements::enable_optional_device_settings
            )
            .emplace_dependency(
                core::renderer::Swapchain::Requirements::require_device_settings
            )
            .emplace_dependency(
                core::renderer::ModelLayout::Requirements::require_device_settings
            )
    );

    app_builder.use([create_framebuffer_size_getter = m_create_framebuffer_size_getter](
                           App&                          app,
                           const vk::UniqueSurfaceKHR&   surface,
                           const core::renderer::Device& device
                       ) {
        app.resources.emplace<core::renderer::SwapchainHolder>(
            surface.get(),
            device,
            create_framebuffer_size_getter
                ? std::invoke(create_framebuffer_size_getter, app)
                : nullptr
        );
    });

    app_builder.use([](App&                            app,
                          const core::renderer::Instance& instance,
                          const core::renderer::Device&   device) {
        app.resources.emplace<core::renderer::Allocator>(instance, device);
    });

    SPDLOG_TRACE("Added Renderer plugin group");
}

}   // namespace plugins::renderer
