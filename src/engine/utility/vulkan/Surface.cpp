#include "Surface.hpp"

namespace engine::renderer::vulkan {

//////////////////////////////////
///----------------------------///
///  Surface   IMPLEMENTATION  ///
///----------------------------///
//////////////////////////////////

Surface::Surface(vk::Instance t_instance, vk::SurfaceKHR t_surface) noexcept
    : m_instance{ t_instance },
      m_surface{ t_surface }
{}

Surface::Surface(Surface&& t_other) noexcept
    : m_instance{ t_other.m_instance },
      m_surface{ t_other.m_surface }
{
    t_other.m_surface = nullptr;
}

Surface::~Surface() noexcept
{
    if (m_surface) {
        m_instance.destroy(m_surface);
    }
}

auto Surface::operator*() const noexcept -> vk::SurfaceKHR
{
    return m_surface;
}

}   // namespace engine::renderer::vulkan
