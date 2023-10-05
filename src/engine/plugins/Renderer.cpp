#include "Renderer.hpp"

#include <thread>

#include "engine/plugins/renderer/Device.hpp"
#include "engine/plugins/renderer/helpers.hpp"
#include "engine/plugins/renderer/Instance.hpp"
#include "engine/plugins/renderer/RenderFrame.hpp"
#include "engine/plugins/renderer/Swapchain.hpp"
#include "engine/plugins/window/Window.hpp"

namespace engine::plugins {

auto Renderer::setup(App::Context& t_context) noexcept -> void
{
    using namespace renderer;
    using namespace window;

    if (!t_context.contains<Window>()) {
        return;
    }
    auto& window{ t_context.get<Window>() };

    auto expected_instance{ Instance::create_default() };
    if (!expected_instance) {
        return;
    }
    auto& instance{ t_context.emplace<Instance>(std::move(*expected_instance)
    ) };

    utils::vulkan::Surface surface{ *instance,
                                    window.create_vulkan_surface(*instance) };

    auto optional_device{ Device::create_default(
        instance, *surface, helpers::choose_physical_device(*instance, *surface)
    ) };
    if (!optional_device.has_value()) {
        return;
    }
    auto& device{ t_context.emplace<Device>(std::move(*optional_device)) };

    t_context.emplace<Swapchain>(std::move(surface), device, nullptr);

    auto optional_render_frame{ RenderFrame::create(
        device, std::max(std::jthread::hardware_concurrency(), 1u)
    ) };
    if (!optional_render_frame.has_value()) {
        return;
    }
    t_context.emplace<RenderFrame>(std::move(*optional_render_frame));
}

}   // namespace engine::plugins
