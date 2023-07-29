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
    -> Result
{
    m_swap_chain = vulkan::SwapChain::create(
        *m_surface,
        m_render_device.physical_device(),
        m_render_device.graphics_queue_family_index(),
        m_render_device.present_queue_family_index(),
        *m_render_device,
        t_framebuffer_size
    );
    if (!m_swap_chain.has_value()) {
        return Result::eFailure;
    }
    return Result::eSuccess;
}

}   // namespace engine
