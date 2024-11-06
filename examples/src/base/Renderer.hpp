#pragma once

#include <core/renderer/base/device/Device.hpp>
#include <core/renderer/base/swapchain/SwapchainHolder.hpp>
#include <core/renderer/scene/Scene.hpp>
#include <core/window/Window.hpp>

namespace examples::base {

struct Renderer {
    constexpr static uint32_t frame_count_v{ 1 };

    explicit Renderer(
        const core::window::Window&            window,
        const core::renderer::base::Device&    device,
        core::renderer::base::SwapchainHolder& swapchain_holder
    );

    template <std::invocable<uint32_t, vk::Extent2D, vk::CommandBuffer, const core::gfx::Camera&> Recorder>
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

#include "Renderer.inl"
