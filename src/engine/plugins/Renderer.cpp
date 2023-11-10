#include "Renderer.hpp"

#include <spdlog/spdlog.h>

#include "engine/plugins/renderer/Device.hpp"
#include "engine/plugins/renderer/helpers.hpp"
#include "engine/plugins/renderer/Instance.hpp"
#include "engine/plugins/renderer/RenderFrame.hpp"
#include "engine/plugins/renderer/Swapchain.hpp"
#include "engine/plugins/window/Window.hpp"

using namespace engine;
using namespace window;
using namespace renderer;

namespace {

[[nodiscard]] auto create_instance(Window& t_window)
    -> tl::optional<std::tuple<Window&, Instance>>
{
    return Instance::create_default()
        .transform([&](Instance&& t_instance) {
            return tl::make_optional(
                std::make_tuple(std::ref(t_window), std::move(t_instance))
            );
        })
        .value_or(tl::nullopt);
}

[[nodiscard]] auto inject_instance(App::Store& t_store)
{
    return [&](std::tuple<Window&, Instance>&& t_pack) {
        return std::make_tuple(
            std::ref(std::get<Window&>(t_pack)),
            std::cref(
                t_store.emplace<Instance>(std::move(std::get<Instance>(t_pack)))
            )
        );
    };
}

[[nodiscard]] auto create_surface(std::tuple<Window&, const Instance&>&& t_pack)
    -> tl::optional<std::tuple<const Instance&, vk::SurfaceKHR>>
{
    vk::SurfaceKHR surface{ std::get<Window&>(t_pack).create_vulkan_surface(
        *std::get<const Instance&>(t_pack)
    ) };
    if (!surface) {
        return tl::nullopt;
    }
    return std::make_tuple(
        std::cref(std::get<const Instance&>(t_pack)), surface
    );
}

[[nodiscard]] auto create_device(
    std::tuple<const Instance&, vk::SurfaceKHR> t_pack
)
{
    return Device::create_default(
               std::get<const Instance&>(t_pack),
               std::get<vk::SurfaceKHR>(t_pack),
               helpers::choose_physical_device(
                   *std::get<const Instance&>(t_pack),
                   std::get<vk::SurfaceKHR>(t_pack)
               )
    )
        .transform([&](Device&& t_device) {
            return std::make_tuple(
                *std::get<const Instance&>(t_pack),
                std::get<vk::SurfaceKHR>(t_pack),
                std::move(t_device)
            );
        });
}

[[nodiscard]] auto inject_device(App::Store& t_store)
{
    return [&t_store](std::tuple<vk::Instance, vk::SurfaceKHR, Device>&& t_pack
           ) {
        return std::make_tuple(
            std::get<vk::Instance>(t_pack),
            std::get<vk::SurfaceKHR>(t_pack),
            std::ref(t_store.emplace<Device>(std::move(std::get<Device>(t_pack))
            ))
        );
    };
}

[[nodiscard]] auto inject_swapchain(App::Store& t_store)
{
    return [&](std::tuple<vk::Instance, vk::SurfaceKHR, Device&>&& t_pack
           ) -> Device& {
        t_store.emplace<Swapchain>(
            vulkan::Surface{ std::get<vk::Instance>(t_pack),
                             std::get<vk::SurfaceKHR>(t_pack) },
            std::get<Device&>(t_pack),
            nullptr
        );
        return std::get<Device&>(t_pack);
    };
}

[[nodiscard]] auto create_render_frame(Device& t_device)
{
    return RenderFrame::create(
        t_device, std::max(std::jthread::hardware_concurrency(), 1u)
    );
}

[[nodiscard]] auto inject_render_frame(App::Store& t_store)
{
    return [&](RenderFrame&& t_render_frame) {
        t_store.emplace<RenderFrame>(std::move(t_render_frame));
    };
}

}   // namespace

namespace engine::plugins {

auto Renderer::operator()(App::Store& t_store) noexcept -> void
{
    t_store.find<Window>()
        .and_then(create_instance)
        .transform(inject_instance(t_store))
        .and_then(create_surface)
        .and_then(create_device)
        .transform(inject_device(t_store))
        .transform(inject_swapchain(t_store))
        .and_then(create_render_frame)
        .transform(inject_render_frame(t_store))
        .transform([](auto) { SPDLOG_TRACE("Added Renderer plugin"); });
}

}   // namespace engine::plugins
