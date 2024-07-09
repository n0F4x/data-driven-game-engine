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

namespace plugins {

template <renderer::SurfaceProviderConcept SurfaceProvider>
auto Renderer::operator()(Store& store, const renderer::Options<SurfaceProvider>& options)
    const -> void
{
    config::vulkan::init();

    const auto system_info_result{ vkb::SystemInfo::get_system_info() };
    if (!system_info_result.has_value()) {
        SPDLOG_ERROR(system_info_result.error().message());
        return;
    }
    const auto& system_info{ system_info_result.value() };

    if (!renderer::default_required_instance_settings_are_available(system_info)
        || !Allocator::Requirements::required_instance_settings_are_available(system_info)
        || !Swapchain::Requirements::required_instance_settings_are_available(system_info))
    {
        return;
    }

    vkb::InstanceBuilder instance_builder;
    instance_builder.require_api_version(options.required_vulkan_version());
    renderer::enable_default_instance_settings(system_info, instance_builder);
    Allocator::Requirements::enable_instance_settings(system_info, instance_builder);
    Swapchain::Requirements::enable_instance_settings(system_info, instance_builder);

    const auto instance_result{ instance_builder.build() };
    if (!instance_result.has_value()) {
        SPDLOG_ERROR(instance_result.error().message());
        return;
    }

    auto& instance{ store.emplace<Instance>(instance_result.value()) };
    config::vulkan::init(instance.get());


    const std::optional<VkSurfaceKHR> surface_result{
        std::invoke(options.surface_provider(), store, instance.get(), nullptr)
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

    auto& device{ store.emplace<Device>(device_result.value()) };
    config::vulkan::init(device.get());


    store.emplace<SwapchainHolder>(
        std::move(surface),
        device,
        options.framebuffer_size_getter()
            ? std::invoke(options.framebuffer_size_getter(), store)
            : nullptr
    );

    store.emplace<Allocator>(instance, device);


    renderer::log_renderer_setup(static_cast<const vkb::Device>(device));
    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace plugins
