#include "RenderFrame.hpp"

#include <limits>
#include <ranges>

#include <spdlog/spdlog.h>

namespace engine::renderer {

auto RenderFrame::create(
    Device&      t_device,
    unsigned int t_thread_count,
    unsigned     t_frame_count
) noexcept -> tl::optional<RenderFrame>
try {
    assert(t_thread_count != 0 && "Thread count must be positive");
    assert(t_frame_count != 0 && "Frame count must be positive");
    if (t_thread_count == 0 || t_frame_count == 0) {
        return tl::nullopt;
    }
    return RenderFrame{
        create_frame_data(t_device, t_thread_count, t_frame_count)
    };
} catch (const vk::Error& t_error) {
    SPDLOG_ERROR(t_error.what());
    return tl::nullopt;
}

auto RenderFrame::reset(vk::Device t_device) noexcept -> vk::Result
{
    m_frame_index = (m_frame_index + 1) % m_frame_data.size();

    const auto result{ t_device.waitForFences(
        *current_frame().fence, true, std::numeric_limits<uint64_t>::max()
    ) };
    if (result != vk::Result::eSuccess) {
        SPDLOG_ERROR(
            "vk::Device::waitForFences failed with error code {}",
            std::to_underlying(result)
        );
        return result;
    }

    for (const auto& thread_data : current_frame().thread_data) {
        t_device.resetCommandPool(*thread_data.command_pool);
    }

    for (auto& update : current_frame().pre_updates) {
        update();
    }
    current_frame().pre_updates.clear();

    return vk::Result::eSuccess;
}

auto RenderFrame::request_command_buffer(
    vk::Device             t_device,
    vk::CommandBufferLevel t_level,
    unsigned               t_thread_id
) -> vk::CommandBuffer
{
    assert(t_thread_id < current_frame().thread_data.size());

    auto& current_thread{ current_frame().thread_data[t_thread_id] };
    if (current_thread.requested_command_buffers
        >= current_thread.command_buffers.size())
    {
        vk::CommandBufferAllocateInfo command_buffer_allocate_info{
            .commandPool        = *current_thread.command_pool,
            .level              = t_level,
            .commandBufferCount = 1
        };

        current_thread.command_buffers.push_back(
            t_device.allocateCommandBuffers(command_buffer_allocate_info)
                .front()
        );
    }

    return current_thread
        .command_buffers[current_thread.requested_command_buffers++];
}

RenderFrame::RenderFrame(std::vector<FrameData>&& t_frame_data)
    : m_frame_data{ std::move(t_frame_data) },
      m_frame_index{ m_frame_data.size() - 1 }
{}

auto RenderFrame::current_frame() noexcept -> RenderFrame::FrameData&
{
    return m_frame_data[m_frame_index];
}

auto RenderFrame::create_frame_data(
    Device&  t_device,
    unsigned t_thread_count,
    unsigned t_frame_count
) -> std::vector<FrameData>
{
    std::vector<FrameData> frame_data;
    frame_data.reserve(t_frame_count);

    for ([[maybe_unused]] auto i : std::ranges::iota_view{ 0u, t_frame_count })
    {
        std::vector<ThreadData> thread_data;
        thread_data.reserve(t_thread_count);
        for ([[maybe_unused]] auto j :
             std::ranges::iota_view{ 0u, t_thread_count })
        {
            vk::CommandPoolCreateInfo command_pool_create_info{
                .flags            = vk::CommandPoolCreateFlagBits::eTransient,
                .queueFamilyIndex = t_device.graphics_queue_family_index()
            };

            thread_data.emplace_back(
                t_device->createCommandPoolUnique(command_pool_create_info),
                std::vector<vk::CommandBuffer>{},
                0
            );
        }

        frame_data.emplace_back(
            std::move(thread_data),
            t_device->createFenceUnique(vk::FenceCreateInfo{}),
            std::vector<std::function<void()>>{}
        );
    }

    return frame_data;
}

}   // namespace engine::renderer
