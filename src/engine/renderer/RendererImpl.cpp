#include "RendererImpl.hpp"

#include <utility>

namespace engine::renderer {

RendererImpl::RendererImpl(const vk::raii::Instance&,
                           vk::raii::SurfaceKHR&& t_surface)
    : m_surface{ std::move(t_surface) }
{
}

}   // namespace engine::renderer
