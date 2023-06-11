#include "Renderer.hpp"

#include <SFML/Window/Vulkan.hpp>

#include "engine/core/vulkan.hpp"

namespace engine {

///////////////////////////////////
///-----------------------------///
///  Renderer   IMPLEMENTATION  ///
///-----------------------------///
///////////////////////////////////
Renderer::Renderer(vk::raii::SurfaceKHR&& t_surface,
                   vk::raii::Instance&&   t_instance)
    : m_device{ std::move(t_instance), t_surface },
      m_swap_chain{ std::move(t_surface) }
{
}

}   // namespace engine
