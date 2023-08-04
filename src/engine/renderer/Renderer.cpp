#include "Renderer.hpp"

#include <thread>

#include "engine/utility/vulkan/helpers.hpp"

namespace engine {

//////////////////////////////////!
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Renderer::Renderer(
    renderer::RenderDevice&&           t_render_device,
    vulkan::Surface&&                  t_surface,
    std::vector<vulkan::CommandPool>&& t_command_pools
) noexcept
    : m_render_device{ std::move(t_render_device) },
      m_surface{ std::move(t_surface) },
      m_command_pools{ std::move(t_command_pools) }
{}

void Renderer::begin_frame() noexcept
{
    m_in_frame = true;
}

void Renderer::end_frame() noexcept
{
    m_in_frame = false;
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

    std::vector<vulkan::CommandPool> command_pools;
    auto hardware_concurrency{ std::jthread::hardware_concurrency() };
    for (unsigned i{}; i < hardware_concurrency; i++) {
        if (auto command_pool{ vulkan::CommandPool::create(
                **render_device,
                vk::CommandPoolCreateFlagBits::eTransient,
                render_device->graphics_queue_family_index()
            ) })
        {
            command_pools.push_back(std::move(*command_pool));
        }
    }
    if (command_pools.empty()) {
        return std::nullopt;
    }

    Renderer renderer{ std::move(*render_device),
                       std::move(t_surface),
                       std::move(command_pools) };
    renderer.set_framebuffer_size(t_framebuffer_size);

    return renderer;
}

}   // namespace engine
