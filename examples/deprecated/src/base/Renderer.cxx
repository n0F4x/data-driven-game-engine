module;

#include <vulkan/vulkan.hpp>

#include <VkBootstrap.h>

module examples.base.Renderer;

import ddge.deprecated.renderer.base.swapchain.SwapchainHolder;
import ddge.deprecated.renderer.scene.Scene;

import ddge.deprecated.window.Window;

import examples.base.init;

examples::base::Renderer::Renderer(
    const ddge::window::Window&            window,
    const ddge::renderer::base::Device&    device,
    ddge::renderer::base::SwapchainHolder& swapchain_holder
)
    : m_device{ device },
      m_swapchain{ swapchain_holder },
      m_command_pool{ init::create_command_pool(
          m_device.get().get(),
          m_device.get().info().get_queue_index(vkb::QueueType::graphics).value()
      ) },
      m_command_buffers{ init::create_command_buffers(
          m_device.get().get(),
          m_command_pool.get(),
          frame_count_v
      ) },
      m_image_acquired_semaphores{
          init::create_semaphores(m_device.get().get(), frame_count_v)
      },
      m_render_finished_semaphores{
          init::create_semaphores(m_device.get().get(), frame_count_v)
      },
      m_render_finished_fences{ init::create_fences(m_device.get().get(), frame_count_v) }
{
    m_swapchain.get().set_framebuffer_size(
        static_cast<vk::Extent2D>(window.framebuffer_size())
    );
}

auto examples::base::Renderer::wait_idle() const -> void
{
    m_device.get()->waitIdle();
}

auto examples::base::Renderer::start_rendering() const -> vk::CommandBuffer
{
    const vk::Extent2D swapchain_extent{ m_swapchain.get().get().value().extent() };

    m_command_buffers[m_frame_index].reset();
    const vk::CommandBuffer command_buffer{ m_command_buffers[m_frame_index] };
    constexpr static vk::CommandBufferBeginInfo command_buffer_begin_info{};

    command_buffer.begin(command_buffer_begin_info);

    command_buffer.setViewport(
        0,
        vk::Viewport{
            .width    = static_cast<float>(swapchain_extent.width),
            .height   = static_cast<float>(swapchain_extent.height),
            .maxDepth = 1.f,
        }
    );
    command_buffer.setScissor(0, vk::Rect2D{ .extent = swapchain_extent });

    return command_buffer;
}

static auto submit_render(
    const vk::Device        device,
    const vk::Queue         queue,
    const vk::Semaphore     wait_semaphore,
    const vk::CommandBuffer command_buffer,
    const vk::Semaphore     signal_semaphore,
    const vk::Fence         signal_fence
) -> void
{
    constexpr vk::PipelineStageFlags wait_pipeline_stage{
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    const vk::SubmitInfo submit_info{
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &wait_semaphore,
        .pWaitDstStageMask    = &wait_pipeline_stage,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &signal_semaphore,
    };

    device.resetFences(signal_fence);
    queue.submit(submit_info, signal_fence);
}

auto examples::base::Renderer::finish_rendering(const vk::CommandBuffer command_buffer)
    -> void
{
    command_buffer.end();

    ::submit_render(
        m_device.get().get(),
        m_device.get().info().get_queue(vkb::QueueType::graphics).value(),
        m_image_acquired_semaphores[m_frame_index].get(),
        m_command_buffers[m_frame_index],
        m_render_finished_semaphores[m_frame_index].get(),
        m_render_finished_fences[m_frame_index].get()
    );

    m_swapchain.get().present(m_render_finished_semaphores[m_frame_index].get());
}
