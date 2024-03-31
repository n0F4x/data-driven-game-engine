#include "Renderer.hpp"

#include <ranges>

#include <spdlog/spdlog.h>

#include "app/Builder.hpp"
#include "core/renderer/base/Allocator.hpp"
#include "core/renderer/base/Device.hpp"
#include "core/renderer/base/Instance.hpp"
#include "core/renderer/base/Swapchain.hpp"
#include "core/utility/vulkan/tools.hpp"
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
        SPDLOG_ERROR("glfwCreateWindowSurface failed with error code {}", std::to_underlying(error_code));
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

[[nodiscard]] static auto instance_extension_names() -> std::set<std::string>
{
    std::set<std::string> result{ std::from_range, instance_extensions() };

    result.insert_range(filter(
        vulkan::available_instance_extensions(),
        Swapchain::required_instance_extensions(),
        Allocator::recommended_instance_extensions()
    ));

    return result;
}

[[nodiscard]] static auto instance_create_info() -> Instance::CreateInfo
{
    return Instance::CreateInfo{
        .application_info = application_info(),
        .layers           = std::set{ std::from_range, layers() },
        .extensions       = instance_extension_names(),
#ifdef core_VULKAN_DEBUG
        .create_debug_messenger = create_debug_messenger
#endif
    };
}

[[nodiscard]] static auto required_device_extension_names() -> std::vector<std::string>
{
    return Swapchain::required_device_extensions() | std::ranges::to<std::vector>();
}

[[nodiscard]] static auto device_extension_names(const vk::PhysicalDevice t_physical_device
) -> std::vector<std::string>
{
    std::vector result{ required_device_extension_names() };

    result.append_range(filter(
        vulkan::available_device_extensions(t_physical_device),
        {},
        Allocator::recommended_device_extensions()
    ));

    return result;
}

[[nodiscard]] static auto device_extension_structs(
    const std::span<const std::string> t_enabled_device_extension_names
)
{
    return Allocator::recommended_device_extension_structs(t_enabled_device_extension_names
    );
}

auto Renderer::operator()(
    app::App::Builder&                  t_builder,
    const SurfaceCreator&               t_create_surface,
    const FramebufferSizeGetterCreator& t_create_framebuffer_size_getter
) const noexcept -> void
{
    auto& instance{ t_builder.store().emplace_or_replace<Instance>(instance_create_info()
    ) };

    vk::UniqueSurfaceKHR surface{
        std::invoke(t_create_surface, t_builder.store(), instance.get(), nullptr),
        instance.get()
    };
    if (!surface) {
        return;
    }

    const vk::PhysicalDevice physical_device{ choose_physical_device(
        instance.get(), surface.get(), required_device_extension_names()
    ) };
    if (!physical_device) {
        return;
    }

    const auto enabled_device_extensions{ device_extension_names(physical_device) };
    const auto enabled_device_extension_structs{
        device_extension_structs(enabled_device_extensions)
    };
    auto& device{
        t_builder.store().emplace_or_replace<Device>(
            surface.get(),
            physical_device,
            Device::CreateInfo{
                               .next       = enabled_device_extension_structs.get().pNext,
                               .extensions = std::set{ std::from_range, enabled_device_extensions } }
        )
    };

    t_builder.store().emplace_or_replace<Swapchain>(
        std::move(surface),
        device,
        t_create_framebuffer_size_getter
            ? std::invoke(t_create_framebuffer_size_getter, t_builder.store())
            : nullptr
    );

    t_builder.store().emplace_or_replace<Allocator>(instance, device);


    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace plugins
