#include "Plugin.hpp"

#include <thread>
#include <tuple>

#include <spdlog/spdlog.h>

#include "engine/window/Window.hpp"

#include "Allocator.hpp"
#include "Device.hpp"
#include "helpers.hpp"
#include "Instance.hpp"
#include "RenderFrame.hpp"
#include "Swapchain.hpp"

using namespace engine::renderer;

namespace engine::renderer {

std::function<VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>
    Plugin::create_surface{ [](Store&                       t_store,
                               VkInstance                   t_instance,
                               const VkAllocationCallbacks* t_allocator) {
        using namespace engine::window;
        return t_store.find<Window>()
            .transform([=](Window& t_window) {
                return t_window.create_vulkan_surface(t_instance, t_allocator);
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
    auto opt_instance{ Instance::create_default() };
    if (!opt_instance) {
        return;
    }
    auto& instance{ t_store.emplace<Instance>(std::move(*opt_instance)) };

    vk::UniqueSurfaceKHR surface{
        std::invoke(t_create_surface, t_store, *instance, nullptr), *instance
    };
    if (!surface) {
        return;
    }

    auto opt_device{ Device::create_default(
        instance, *surface, helpers::choose_physical_device(*instance, *surface)
    ) };
    if (!opt_device) {
        return;
    }
    auto& device{ t_store.emplace<Device>(std::move(*opt_device)) };

    t_store.emplace<Swapchain>(
        std::move(surface),
        device,
        t_create_framebuffer_size_getter
            ? std::invoke(t_create_framebuffer_size_getter, t_store)
            : nullptr
    );

    auto opt_allocator{ Allocator::create_default(instance, device) };
    if (!opt_allocator) {
        return;
    }
    t_store.emplace<Allocator>(std::move(*opt_allocator));

    auto render_frame{ RenderFrame::create(
        device, std::max(std::thread::hardware_concurrency(), 2u)
    ) };
    if (!render_frame) {
        return;
    }
    t_store.emplace<RenderFrame>(std::move(*render_frame));


    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace engine::renderer
