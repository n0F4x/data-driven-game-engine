#include "Renderer.hpp"

#include <ranges>

#include "engine/utility/vulkan/helpers.hpp"

namespace engine {

//////////////////////////////////!
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Renderer::Renderer(
    renderer::RenderDevice&&                                  t_render_device,
    vulkan::Surface&&                                         t_surface,
    std::array<renderer::FrameData, s_max_frames_in_flight>&& t_frame_data
) noexcept
    : m_render_device{ std::move(t_render_device) },
      m_surface{ std::move(t_surface) },
      m_frame_data{ std::move(t_frame_data) }
{}

auto Renderer::set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (!m_rendering) {
        recreate_swap_chain(t_framebuffer_size);
    }
}

auto Renderer::allocate_command_buffer(
    const renderer::CommandBufferAllocateInfo& t_allocate_info,
    size_t                                     t_work_load
) noexcept -> std::expected<renderer::CommandHandle, vk::Result>
{
    if (m_rendering) {
        return std::unexpected{ vk::Result::eNotReady };
    }

    auto command_pool_index{ *std::ranges::min_element(
        std::views::iota(m_frame_data[m_frame_index].command_buffers.size()),
        std::less{},
        [&](const auto& pool_index) {
            size_t sum{};
            for (const auto& buffer :
                 m_frame_data[m_frame_index].command_buffers[pool_index])
            {
                sum += buffer.work_load;
            }
            return sum;
        }
    ) };

    for (uint32_t frame_index{}; frame_index < s_max_frames_in_flight;
         frame_index++)
    {
        auto [result, command_buffer]{ m_render_device->allocateCommandBuffers(
            vk::CommandBufferAllocateInfo{
                .pNext       = t_allocate_info.pNext,
                .commandPool = *m_frame_data[frame_index]
                                    .command_pools[command_pool_index],
                .level              = t_allocate_info.level,
                .commandBufferCount = 1 }
        ) };
        if (result != vk::Result::eSuccess) {
            return std::unexpected{ result };
        }

        m_frame_data[frame_index]
            .command_buffers[command_pool_index]
            .emplace_back(command_buffer[0], t_work_load);
    }

    m_command_map.try_emplace(
        m_next_command_handle,
        renderer::CommandNodeInfo{ .worker_id = command_pool_index,
                                   .index =
                                       m_frame_data[m_frame_index]
                                           .command_buffers[command_pool_index]
                                           .size()
                                       - 1 }
    );

    return m_next_command_handle++;
}

auto Renderer::free_command_buffer(renderer::CommandHandle t_command) noexcept
    -> void
{
    auto info{ get_command_buffer_info(t_command) };
    if (!info.has_value()) {
        return;
    }

    auto [worker_id, index]{ *info };

    auto free_buffer{ [&]() {
        auto& frame_data{ m_frame_data[m_frame_index] };

        m_render_device->freeCommandBuffers(
            *frame_data.command_pools[worker_id],
            frame_data.command_buffers[worker_id][index].command_buffer
        );

        frame_data.command_buffers[worker_id].erase(
            frame_data.command_buffers[worker_id].begin()
            + static_cast<
                std::vector<engine::renderer::CommandNode>::difference_type>(
                index
            )
        );
    } };

    for (auto& pre_update_list : m_pre_updates) {
        pre_update_list.emplace_back(free_buffer);
    }

    free_buffer();

    m_command_map.erase(t_command);
}

auto Renderer::reset() noexcept -> Result
{
    if (m_render_device->waitForFences(
            *m_frame_data[m_frame_index].fence,
            true,
            std::numeric_limits<uint64_t>::max()
        )
        != vk::Result::eSuccess)
    {
        return Result::eFailure;
    }

    for (auto& command_pool : m_frame_data[m_frame_index].command_pools) {
        m_render_device->resetCommandPool(*command_pool);
    }

    return Result::eSuccess;
}

auto Renderer::pre_update() noexcept -> void
{
    for (auto& update : m_pre_updates[m_frame_index]) {
        update();
    }
    m_pre_updates[m_frame_index].clear();
}

auto Renderer::begin_frame() noexcept -> void
{
    m_rendering = true;
}

auto Renderer::end_frame() noexcept -> void
{
    m_rendering   = false;
    m_frame_index = (m_frame_index + 1) % s_max_frames_in_flight;
}

auto Renderer::wait_idle() noexcept -> void
{
    static_cast<void>(m_render_device->waitIdle());
}

auto Renderer::recreate_swap_chain(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (m_render_device->waitIdle() != vk::Result::eSuccess) {
        return;
    }

    auto new_swap_chain{ vulkan::SwapChain::create(
        *m_surface,
        m_render_device.physical_device(),
        m_render_device.graphics_queue_family_index(),
        m_render_device.present_queue_family_index(),
        *m_render_device,
        t_framebuffer_size,
        m_swap_chain.transform(&vulkan::SwapChain::operator*)
    ) };
    m_swap_chain.reset();
    m_swap_chain = std::move(new_swap_chain);
}

auto Renderer::get_command_buffer_info(renderer::CommandHandle t_command
) const noexcept -> std::optional<renderer::CommandNodeInfo>
{
    if (auto iter{ m_command_map.find(t_command) }; iter == m_command_map.end())
    {
        return std::nullopt;
    }
    else {
        return iter->second;
    }
}

auto Renderer::create(
    vulkan::Instance&& t_instance,
    vulkan::Surface&&  t_surface,
    vk::Extent2D       t_framebuffer_size,
    unsigned           t_hardware_concurrency
) noexcept -> std::optional<Renderer>
{
    std::optional<vulkan::DebugUtilsMessenger> debug_messenger{
        vulkan::create_debug_messenger(*t_instance)
            .transform([instance =
                            *t_instance](vk::DebugUtilsMessengerEXT messenger) {
                return vulkan::DebugUtilsMessenger{ instance, messenger };
            })
    };

    auto render_device{ renderer::RenderDevice::create(
        std::move(t_instance), std::move(debug_messenger), *t_surface
    ) };
    if (!render_device.has_value()) {
        return std::nullopt;
    }

    std::array<renderer::FrameData, s_max_frames_in_flight> frame_data;

    for (size_t i{}; i < s_max_frames_in_flight; i++) {
        frame_data[i].command_pools.reserve(t_hardware_concurrency);
        for (unsigned j{}; j < t_hardware_concurrency; j++) {
            if (auto command_pool{ vulkan::CommandPool::create(
                    **render_device,
                    vk::CommandPoolCreateFlagBits::eTransient,
                    render_device->graphics_queue_family_index()
                ) })
            {
                frame_data[i].command_pools.push_back(std::move(*command_pool));
            }
            else {
                return std::nullopt;
            }
        }
    }

    for (uint32_t i{}; i < s_max_frames_in_flight; i++) {
        if (auto [result, fence]{
                (*render_device)->createFence(vk::FenceCreateInfo{}) };
            result == vk::Result::eSuccess)
        {
            frame_data[i].fence = vulkan::Fence{ **render_device, fence };
        }
        else {
            return std::nullopt;
        }
    }

    Renderer renderer{ std::move(*render_device),
                       std::move(t_surface),
                       std::move(frame_data) };
    renderer.set_framebuffer_size(t_framebuffer_size);

    return renderer;
}

}   // namespace engine
