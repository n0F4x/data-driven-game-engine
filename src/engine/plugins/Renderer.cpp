#include "Renderer.hpp"

#include <spdlog/spdlog.h>

#include "engine/plugins/renderer/Device.hpp"
#include "engine/plugins/renderer/helpers.hpp"
#include "engine/plugins/renderer/Instance.hpp"
#include "engine/plugins/renderer/RenderFrame.hpp"
#include "engine/plugins/renderer/Swapchain.hpp"
#include "engine/plugins/window/Window.hpp"

namespace engine::plugins {

auto Renderer::operator()(App::Store& t_store) noexcept -> void
{
    using namespace renderer;
    using namespace window;

    auto window{ t_store.find<Window>() };
    if (!window.has_value()) {
        return;
    }

    auto original_instance{ Instance::create_default() };
    if (!original_instance) {
        return;
    }
    const auto& instance{ t_store.emplace<Instance>(std::move(*original_instance
    )) };

    utils::vulkan::Surface surface{
        *instance, vk::SurfaceKHR{ window->create_vulkan_surface(*instance) }
    };

    auto original_device{ Device::create_default(
        instance, *surface, helpers::choose_physical_device(*instance, *surface)
    ) };
    if (!original_device.has_value()) {
        return;
    }
    auto& device{ t_store.emplace<Device>(std::move(*original_device)) };

    t_store.emplace<Swapchain>(std::move(surface), device, nullptr);

    auto original_render_frame{ RenderFrame::create(
        device, std::max(std::jthread::hardware_concurrency(), 1u)
    ) };
    if (!original_render_frame.has_value()) {
        return;
    }
    t_store.emplace<RenderFrame>(std::move(*original_render_frame));

    SPDLOG_TRACE("Added Renderer plugin");
}

}   // namespace engine::plugins
