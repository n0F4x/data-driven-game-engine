#include "Renderer.hpp"

#include <spdlog/spdlog.h>

#include <VkBootstrap.h>

#include "app/Builder.hpp"
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

using namespace renderer;

static auto create_vulkan_surface(
    GLFWwindow*                  t_window,
    VkInstance                   t_instance,
    const VkAllocationCallbacks* t_allocator
) -> VkSurfaceKHR
{
    VkSurfaceKHR surface{};

    if (vk::Result error_code{
            glfwCreateWindowSurface(t_instance, t_window, t_allocator, &surface) };
        error_code != vk::Result::eSuccess)
    {
        SPDLOG_ERROR(
            "glfwCreateWindowSurface failed with error code {}",
            std::to_underlying(error_code)
        );
    }

    return surface;
}

std::function<VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>
    Renderer::create_default_surface{ [](Store&                       t_store,
                                         VkInstance                   t_instance,
                                         const VkAllocationCallbacks* t_allocator) {
        using namespace core::window;
        return t_store.find<Window>()
            .transform([=](const Window& t_window) {
                return create_vulkan_surface(t_window.get(), t_instance, t_allocator);
            })
            .or_else([] {
                SPDLOG_WARN(
                    "Default window could not be found in store. "
                    "Consider using another surface creator than the default."
                );
            })
            .value_or(nullptr);
    } };

static auto log_renderer_setup(const vkb::Device& t_device)
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

        std::string enabled_extensions{ "\nEnabled device extensions:" };
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

auto Renderer::operator()(
    app::App::Builder&                  t_builder,
    const SurfaceCreator&               t_create_surface,
    const FramebufferSizeGetterCreator& t_create_framebuffer_size_getter
) const noexcept -> void
{
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

    vkb::InstanceBuilder builder;
    enable_default_instance_settings(system_info, builder);
    Allocator::Requirements::enable_instance_settings(system_info, builder);
    Swapchain::Requirements::enable_instance_settings(system_info, builder);

    const auto instance_result{ builder.build() };
    if (!instance_result.has_value()) {
        SPDLOG_ERROR(instance_result.error().message());
        return;
    }

    auto& instance{ t_builder.store().emplace<Instance>(instance_result.value()) };


    vk::UniqueSurfaceKHR surface{
        std::invoke(t_create_surface, t_builder.store(), instance.get(), nullptr),
        instance.get()
    };
    if (!surface) {
        return;
    }


    vkb::PhysicalDeviceSelector physical_device_selector(
        static_cast<const vkb::Instance>(instance), surface.get()
    );
    Allocator::Requirements::require_device_settings(physical_device_selector);
    Swapchain::Requirements::require_device_settings(physical_device_selector);
    RenderModel2::Requirements::require_device_settings(physical_device_selector);

    auto physical_device_result{ physical_device_selector.select() };
    if (!physical_device_result.has_value()) {
        SPDLOG_ERROR(physical_device_result.error().message());
        return;
    }

    Allocator::Requirements::enable_optional_device_settings(physical_device_result.value(
    ));
    Swapchain::Requirements::enable_optional_device_settings(physical_device_result.value(
    ));

    vkb::DeviceBuilder device_builder{ physical_device_result.value() };
    const auto         device_result{ device_builder.build() };
    if (!device_result.has_value()) {
        SPDLOG_ERROR(device_result.error().message());
        return;
    }

    auto& device{ t_builder.store().emplace<Device>(device_result.value()) };


    t_builder.store().emplace<Swapchain>(
        std::move(surface),
        device,
        t_create_framebuffer_size_getter
            ? std::invoke(t_create_framebuffer_size_getter, t_builder.store())
            : nullptr
    );

    t_builder.store().emplace<Allocator>(instance, device);


    log_renderer_setup(static_cast<const vkb::Device>(device));
    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace plugins
