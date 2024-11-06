#pragma once

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
