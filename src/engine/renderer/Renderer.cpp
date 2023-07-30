#include "Renderer.hpp"

namespace engine {

//////////////////////////////////!
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////

Renderer::Renderer(
    renderer::RenderDevice&& t_render_device,
    vulkan::Surface&&        t_surface
) noexcept
    : m_render_device{ std::move(t_render_device) },
      m_surface{ std::move(t_surface) }
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

void Renderer::recreate_swap_chain(vk::Extent2D t_framebuffer_size) noexcept
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

}   // namespace engine
