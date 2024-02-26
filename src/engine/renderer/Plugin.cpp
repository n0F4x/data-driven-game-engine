#include "Plugin.hpp"

#include <tuple>

#include <spdlog/spdlog.h>

#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/base/Device.hpp"
#include "engine/renderer/base/helpers.hpp"
#include "engine/renderer/base/Instance.hpp"
#include "engine/renderer/base/Swapchain.hpp"
#include "engine/window/Window.hpp"

using namespace engine::renderer;

namespace engine::renderer {

static auto create_vulkan_surface(
    GLFWwindow*                  t_window,
    VkInstance                   t_instance,
    const VkAllocationCallbacks* t_allocator
) -> VkSurfaceKHR
{
    VkSurfaceKHR surface{};

    if (auto error_code{
            glfwCreateWindowSurface(t_instance, t_window, t_allocator, &surface) };
        error_code != VK_SUCCESS)
    {
        SPDLOG_ERROR("glfwCreateWindowSurface failed with error code {}", error_code);
    }

    return surface;
}

std::function<VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>
    Plugin::create_default_surface{ [](Store&                       t_store,
                                       VkInstance                   t_instance,
                                       const VkAllocationCallbacks* t_allocator) {
        using namespace engine::window;
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

auto Plugin::operator()(
    Store&                              t_store,
    const SurfaceCreator&               t_create_surface,
    const FramebufferSizeGetterCreator& t_create_framebuffer_size_getter
) const noexcept -> void
{
    auto& instance{ t_store.emplace_or_replace<Instance>() };

    vk::UniqueSurfaceKHR surface{
        std::invoke(t_create_surface, t_store, *instance, nullptr), *instance
    };
    if (!surface) {
        return;
    }

    auto& device{ t_store.emplace_or_replace<Device>(
        surface.get(), helpers::choose_physical_device(*instance, *surface)
    ) };

    t_store.emplace_or_replace<Swapchain>(
        std::move(surface),
        device,
        t_create_framebuffer_size_getter
            ? std::invoke(t_create_framebuffer_size_getter, t_store)
            : nullptr
    );

    t_store.emplace_or_replace<Allocator>(instance, device);


    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace engine::renderer
