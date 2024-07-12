#include <spdlog/spdlog.h>

#include <VkBootstrap.h>

#include "core/config/vulkan.hpp"
#include "core/renderer/base/allocator/Requirements.hpp"
#include "core/renderer/base/device/Device.hpp"
#include "core/renderer/base/instance/Instance.hpp"
#include "core/renderer/base/swapchain/Requirements.hpp"
#include "core/renderer/model/Requirements.hpp"
#include "core/window/Window.hpp"
#include "plugins/renderer/helpers.hpp"

using namespace core;
using namespace core::renderer;

namespace plugins::renderer {

template <SurfaceProviderConcept SurfaceProvider>
auto BasicRenderer<SurfaceProvider>::require_vulkan_version(
    const uint32_t t_major,
    const uint32_t t_minor,
    const uint32_t t_patch
) noexcept -> BasicRenderer&
{
    m_required_vulkan_version = VK_MAKE_API_VERSION(0, t_major, t_minor, t_patch);
    return *this;
}

template <SurfaceProviderConcept SurfaceProvider>
template <SurfaceProviderConcept NewSurfaceProvider>
auto BasicRenderer<SurfaceProvider>::set_surface_provider(
    NewSurfaceProvider&& t_surface_provider
) -> BasicRenderer<NewSurfaceProvider>
{
    return BasicRenderer<NewSurfaceProvider>{ m_required_vulkan_version,
                                              std::move(t_surface_provider),
                                              std::move(m_create_framebuffer_size_getter
                                              ) };
}

template <SurfaceProviderConcept SurfaceProvider>
auto BasicRenderer<SurfaceProvider>::set_framebuffer_size_getter(
    FramebufferSizeGetterCreator t_framebuffer_size_callback
) -> BasicRenderer&
{
    m_create_framebuffer_size_getter = std::move(t_framebuffer_size_callback);
    return *this;
}

template <SurfaceProviderConcept SurfaceProvider>
auto BasicRenderer<SurfaceProvider>::required_vulkan_version() const noexcept -> uint32_t
{
    return m_required_vulkan_version;
}

template <SurfaceProviderConcept SurfaceProvider>
auto BasicRenderer<SurfaceProvider>::surface_provider() const noexcept
    -> const SurfaceProvider&
{
    return m_surface_provider;
}

template <SurfaceProviderConcept SurfaceProvider>
auto BasicRenderer<SurfaceProvider>::framebuffer_size_getter() const noexcept
    -> const FramebufferSizeGetterCreator&
{
    return m_create_framebuffer_size_getter;
}

template <SurfaceProviderConcept SurfaceProvider>
auto BasicRenderer<SurfaceProvider>::operator()(App& app) const -> void
{
    config::vulkan::init();

    const auto system_info_result{ vkb::SystemInfo::get_system_info() };
    if (!system_info_result.has_value()) {
        SPDLOG_ERROR(system_info_result.error().message());
        return;
    }
    const auto& system_info{ system_info_result.value() };

    if (!default_required_instance_settings_are_available(system_info)
        || !Allocator::Requirements::required_instance_settings_are_available(system_info)
        || !Swapchain::Requirements::required_instance_settings_are_available(system_info))
    {
        return;
    }

    vkb::InstanceBuilder instance_builder;
    instance_builder.require_api_version(m_required_vulkan_version);
    enable_default_instance_settings(system_info, instance_builder);
    Allocator::Requirements::enable_instance_settings(system_info, instance_builder);
    Swapchain::Requirements::enable_instance_settings(system_info, instance_builder);

    const auto instance_result{ instance_builder.build() };
    if (!instance_result.has_value()) {
        SPDLOG_ERROR(instance_result.error().message());
        return;
    }

    auto& instance{ app.plugins().emplace<Instance>(instance_result.value()) };
    config::vulkan::init(instance.get());


    const std::optional<VkSurfaceKHR> surface_result{
        std::invoke(m_surface_provider, app, instance.get(), nullptr)
    };
    if (!surface_result.has_value()) {
        SPDLOG_ERROR("Vulkan surface creation failed");
        return;
    }
    vk::UniqueSurfaceKHR surface{ surface_result.value(), instance.get() };


    vkb::PhysicalDeviceSelector physical_device_selector(
        static_cast<const vkb::Instance>(instance), surface.get()
    );
    Allocator::Requirements::require_device_settings(physical_device_selector);
    Swapchain::Requirements::require_device_settings(physical_device_selector);
    RenderModel::Requirements::require_device_settings(physical_device_selector);

    auto physical_device_result{ physical_device_selector.select() };
    if (!physical_device_result.has_value()) {
        SPDLOG_ERROR(physical_device_result.error().message());
        return;
    }

    Allocator::Requirements::enable_optional_device_settings(physical_device_result.value(
    ));
    Swapchain::Requirements::enable_optional_device_settings(physical_device_result.value(
    ));

    const vkb::DeviceBuilder device_builder{ physical_device_result.value() };
    const auto               device_result{ device_builder.build() };
    if (!device_result.has_value()) {
        SPDLOG_ERROR(device_result.error().message());
        return;
    }

    auto& device{ app.plugins().emplace<Device>(device_result.value()) };
    config::vulkan::init(device.get());


    app.plugins().emplace<SwapchainHolder>(
        std::move(surface),
        device,
        m_create_framebuffer_size_getter
            ? std::invoke(m_create_framebuffer_size_getter, app)
            : nullptr
    );

    app.plugins().emplace<Allocator>(instance, device);


    log_renderer_setup(static_cast<const vkb::Device>(device));
    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace plugins::renderer
