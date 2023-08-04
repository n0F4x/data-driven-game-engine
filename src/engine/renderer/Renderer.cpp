#include "Renderer.hpp"

#include <ranges>
#include <thread>

#include "engine/utility/vulkan/helpers.hpp"

namespace engine {

//////////////////////////////////!
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Renderer::Renderer(
    renderer::RenderDevice&& t_render_device,
    vulkan::Surface&&        t_surface,
    std::array<std::vector<vulkan::CommandPool>, s_MAX_FRAMES_IN_FLIGHT>&&
                                 t_command_pools,
    std::vector<vulkan::Fence>&& t_fences
) noexcept
    : m_render_device{ std::move(t_render_device) },
      m_surface{ std::move(t_surface) },
      m_command_pools{ std::move(t_command_pools) },
      m_fences{ std::move(t_fences) }
{}

auto Renderer::begin_frame() noexcept -> Result
{
    if (m_render_device->waitForFences(
            *m_fences[m_frame_index], true, std::numeric_limits<uint64_t>::max()
        )
        != vk::Result::eSuccess)
    {
        return Result::eFailure;
    }
    m_in_frame = true;
    return Result::eSuccess;
}

auto Renderer::end_frame() noexcept -> void
{
    m_in_frame    = false;
    m_frame_index = (m_frame_index + 1) % s_MAX_FRAMES_IN_FLIGHT;
}

auto Renderer::set_framebuffer_size(vk::Extent2D t_framebuffer_size) noexcept
    -> void
{
    if (!m_in_frame) {
        recreate_swap_chain(t_framebuffer_size);
    }
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

auto Renderer::command_pools_per_frame() const noexcept -> size_t
{
    return m_command_pools.size() / s_MAX_FRAMES_IN_FLIGHT;
}

auto Renderer::wait_idle() noexcept -> void
{
    static_cast<void>(m_render_device->waitIdle());
}

auto Renderer::create(
    vulkan::Instance&& t_instance,
    vulkan::Surface&&  t_surface,
    vk::Extent2D       t_framebuffer_size
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

    std::array<std::vector<vulkan::CommandPool>, s_MAX_FRAMES_IN_FLIGHT>
         command_pools;
    auto hardware_concurrency{
        std::min(std::jthread::hardware_concurrency(), 1u)
        * s_MAX_FRAMES_IN_FLIGHT
    };
    for (size_t i{}; i < s_MAX_FRAMES_IN_FLIGHT; i++) {
        command_pools[i].reserve(hardware_concurrency);
        for (unsigned j{}; j < hardware_concurrency; j++) {
            if (auto command_pool{ vulkan::CommandPool::create(
                    **render_device,
                    vk::CommandPoolCreateFlagBits::eTransient,
                    render_device->graphics_queue_family_index()
                ) })
            {
                command_pools[i].push_back(std::move(*command_pool));
            }
        }
        if (command_pools[i].empty()) {
            return std::nullopt;
        }
    }

    std::vector<vulkan::Fence> fences;
    fences.reserve(s_MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i{}; i < s_MAX_FRAMES_IN_FLIGHT; i++) {
        if (auto [result, fence]{
                (*render_device)->createFence(vk::FenceCreateInfo{}) };
            result == vk::Result::eSuccess)
        {
            fences.emplace_back(**render_device, fence);
        }
    }
    if (fences.size() < s_MAX_FRAMES_IN_FLIGHT) {
        return std::nullopt;
    }

    Renderer renderer{ std::move(*render_device),
                       std::move(t_surface),
                       std::move(command_pools),
                       std::move(fences) };
    renderer.set_framebuffer_size(t_framebuffer_size);

    return renderer;
}

}   // namespace engine
