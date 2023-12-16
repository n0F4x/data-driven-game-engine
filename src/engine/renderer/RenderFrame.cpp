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
{
    assert(t_frame_count != 0 && "Frame count must be positive");
    if (t_frame_count == 0) {
        return tl::nullopt;
    }

    std::vector<FrameData> frame_data;
    frame_data.reserve(t_frame_count);

    for ([[maybe_unused]] auto i : std::ranges::iota_view{ 0u, t_frame_count })
    {
        std::vector<ThreadData> thread_data;
        thread_data.reserve(t_thread_count);
        for ([[maybe_unused]] auto j :
             std::ranges::iota_view{ 0u, t_thread_count })
        {
            auto [result, command_pool]{ t_device->createCommandPoolUnique(
                vk::CommandPoolCreateInfo{
                    .flags = vk::CommandPoolCreateFlagBits::eTransient,
                    .queueFamilyIndex = t_device.graphics_queue_family_index() }
            ) };
            if (result != vk::Result::eSuccess) {
                SPDLOG_ERROR(
                    "vk::Device::createCommandPool failed with error code {}",
                    std::to_underlying(result)
                );
                return tl::nullopt;
            }

            thread_data.emplace_back(
                std::move(command_pool), std::vector<vk::CommandBuffer>{}, 0
            );
        }

        auto [result, fence]{ t_device->createFenceUnique(vk::FenceCreateInfo{}
        ) };
        if (result != vk::Result::eSuccess) {
            SPDLOG_ERROR(
                "vk::Device::createFence failed with error code {}",
                std::to_underlying(result)
            );
            return tl::nullopt;
        }

        frame_data.emplace_back(
            std::move(thread_data),
            std::move(fence),
            std::vector<std::function<void()>>{}
        );
    }

    return RenderFrame{ std::move(frame_data) };
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
) noexcept -> std::expected<vk::CommandBuffer, vk::Result>
{
    assert(t_thread_id < current_frame().thread_data.size());

    auto& current_thread{ current_frame().thread_data[t_thread_id] };
    if (current_thread.requested_command_buffers
        >= current_thread.command_buffers.size())
    {
        const auto [result, command_buffer]{
            t_device.allocateCommandBuffers(vk::CommandBufferAllocateInfo{
                .commandPool        = *current_thread.command_pool,
                .level              = t_level,
                .commandBufferCount = 1 })
        };
        if (result != vk::Result::eSuccess) {
            SPDLOG_ERROR(
                "vk::Device::allocateCommandBuffers failed with error code {}",
                std::to_underlying(result)
            );
            return std::unexpected{ result };
        }
        current_thread.command_buffers.push_back(command_buffer.front());
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

}   // namespace engine::renderer
