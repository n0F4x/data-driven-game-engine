module;

#include <concepts>
#include <cstdint>
#include <optional>
#include <vector>

export module examples.base.Renderer;

import vulkan_hpp;

import core.gfx.Camera;
import core.renderer.base.device.Device;
import core.renderer.base.swapchain.SwapchainHolder;
import core.renderer.scene.Scene;

import core.window.Window;

namespace examples::base {

export struct Renderer {
    constexpr static uint32_t frame_count_v{ 1 };

    explicit Renderer(
        const core::window::Window&            window,
        const core::renderer::base::Device&    device,
        core::renderer::base::SwapchainHolder& swapchain_holder
    );

    template <
        std::invocable<uint32_t, vk::Extent2D, vk::CommandBuffer, const core::gfx::Camera&>
            Recorder>
    auto render(
        vk::Extent2D      framebuffer_size,
        core::gfx::Camera camera,
        Recorder&&        recorder
    ) -> void;

    auto wait_idle() const -> void;

private:
    auto start_rendering() const -> vk::CommandBuffer;
    auto finish_rendering(vk::CommandBuffer command_buffer) -> void;


    std::reference_wrapper<const core::renderer::base::Device>    m_device;
    std::reference_wrapper<core::renderer::base::SwapchainHolder> m_swapchain;
    vk::UniqueCommandPool                                         m_command_pool;
    std::vector<vk::CommandBuffer>                                m_command_buffers;
    std::vector<vk::UniqueSemaphore> m_image_acquired_semaphores;
    std::vector<vk::UniqueSemaphore> m_render_finished_semaphores;
    std::vector<vk::UniqueFence>     m_render_finished_fences;
    uint32_t                         m_frame_index{};
    std::optional<uint32_t>          m_renderable_image_index;
};

}   // namespace examples::base

template <std::invocable<uint32_t, vk::Extent2D, vk::CommandBuffer, const core::gfx::Camera&>
              Recorder>
auto examples::base::Renderer::render(
    const vk::Extent2D framebuffer_size,
    core::gfx::Camera  camera,
    Recorder&&         recorder
) -> void
{
    m_swapchain.get().set_framebuffer_size(framebuffer_size);
    if (!m_swapchain.get().get().has_value()) {
        return;
    }

    std::ignore = m_device.get()->waitForFences(
        m_render_finished_fences[m_frame_index].get(),
        vk::True,
        std::numeric_limits<uint64_t>::max()
    );

    m_renderable_image_index = m_swapchain.get().acquire_next_image(
        m_image_acquired_semaphores[m_frame_index].get()
    );

    if (m_renderable_image_index.has_value()) {
        const vk::Extent2D swapchain_extent{ m_swapchain.get().get().value().extent() };

        camera.set_perspective_projection(
            50.f,
            static_cast<float>(swapchain_extent.width)
                / static_cast<float>(swapchain_extent.height),
            0.01f,
            10000.f
        );

        const vk::CommandBuffer command_buffer{ start_rendering() };

        std::invoke(
            std::forward<Recorder>(recorder),
            m_renderable_image_index.value(),
            swapchain_extent,
            command_buffer,
            camera
        );

        finish_rendering(command_buffer);
    }

    m_frame_index = (m_frame_index + 1) % frame_count_v;
}
