#include "Plugin.hpp"

#include <thread>
#include <tuple>

#include <spdlog/spdlog.h>

#include "engine/window/Window.hpp"

#include "Device.hpp"
#include "helpers.hpp"
#include "Instance.hpp"
#include "RenderFrame.hpp"
#include "Swapchain.hpp"

using namespace engine;
using namespace engine::window;
using namespace engine::renderer;

namespace {

[[nodiscard]] auto create_instance() -> tl::optional<Instance>
{
    return Instance::create_default()
        .transform(
            []<typename Instance>(Instance&& t_instance
            ) -> tl::optional<Instance> {
                return std::forward<Instance>(t_instance);
            }
        )
        .value_or(tl::nullopt);
}

[[nodiscard]] auto inject_instance(Store& t_store)
{
    return [&]<typename Instance>(Instance&& t_instance) -> Instance& {
        return t_store.emplace<Instance>(std::forward<Instance>(t_instance));
    };
}

template <typename TSurfaceCreator>
[[nodiscard]] auto
    create_surface(Store& t_store, TSurfaceCreator&& t_create_surface)
{
    return
        [&](const Instance& t_instance
        ) -> tl::optional<std::tuple<const Instance&, vk::UniqueSurfaceKHR>> {
            vk::SurfaceKHR surface{ std::invoke(
                std::forward<TSurfaceCreator>(t_create_surface),
                t_store,
                *t_instance,
                nullptr
            ) };
            if (!surface) {
                return tl::nullopt;
            }
            return std::make_tuple(
                std::cref(t_instance),
                vk::UniqueSurfaceKHR{ surface, *t_instance }
            );
        };
}

[[nodiscard]] auto create_device(
    std::tuple<const Instance&, vk::UniqueSurfaceKHR> t_pack
)
{
    return Device::create_default(
               std::get<const Instance&>(t_pack),
               *std::get<vk::UniqueSurfaceKHR>(t_pack),
               helpers::choose_physical_device(
                   *std::get<const Instance&>(t_pack),
                   *std::get<vk::UniqueSurfaceKHR>(t_pack)
               )
    )
        .transform([&](Device&& t_device) {
            return std::make_tuple(
                *std::get<const Instance&>(t_pack),
                std::move(std::get<vk::UniqueSurfaceKHR>(t_pack)),
                std::move(t_device)
            );
        });
}

[[nodiscard]] auto inject_device(Store& t_store)
{
    return [&t_store](
               std::tuple<vk::Instance, vk::UniqueSurfaceKHR, Device>&& t_pack
           ) {
        return std::make_tuple(
            std::get<vk::Instance>(t_pack),
            std::move(std::get<vk::UniqueSurfaceKHR>(t_pack)),
            std::ref(t_store.emplace<Device>(std::move(std::get<Device>(t_pack))
            ))
        );
    };
}

template <typename FramebufferSizeGetterCreator>
[[nodiscard]] auto inject_swapchain(
    Store&                         t_store,
    FramebufferSizeGetterCreator&& t_create_framebuffer_size_getter
)
{
    return [&](std::tuple<vk::Instance, vk::UniqueSurfaceKHR, Device&>&& t_pack
           ) -> Device& {
        t_store.emplace<Swapchain>(
            std::move(std::get<vk::UniqueSurfaceKHR>(t_pack)),
            std::get<Device&>(t_pack),
            t_create_framebuffer_size_getter
                ? std::invoke(
                      std::forward<FramebufferSizeGetterCreator>(
                          t_create_framebuffer_size_getter
                      ),
                      t_store
                  )
                : nullptr
        );
        return std::get<Device&>(t_pack);
    };
}

[[nodiscard]] auto create_render_frame(Device& t_device)
{
    return RenderFrame::create(
        t_device, std::max(std::thread::hardware_concurrency(), 2u)
    );
}

[[nodiscard]] auto inject_render_frame(Store& t_store)
{
    return [&](RenderFrame&& t_render_frame) {
        t_store.emplace<RenderFrame>(std::move(t_render_frame));
    };
}

}   // namespace

namespace engine::renderer {

std::function<VkSurfaceKHR(Store&, VkInstance, const VkAllocationCallbacks*)>
    Plugin::create_surface{ [](Store&                       t_store,
                               VkInstance                   t_instance,
                               const VkAllocationCallbacks* t_allocator) {
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
    create_instance()
        .transform(inject_instance(t_store))
        .and_then(::create_surface(t_store, t_create_surface))
        .and_then(create_device)
        .transform(inject_device(t_store))
        .transform(inject_swapchain(t_store, t_create_framebuffer_size_getter))
        .and_then(create_render_frame)
        .transform(inject_render_frame(t_store))
        .transform([](auto) { SPDLOG_TRACE("Added Renderer plugin"); });
}

}   // namespace engine::renderer
