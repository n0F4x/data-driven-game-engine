#include "SwapChain.hpp"

namespace engine::renderer {

////////////////////////////////////
///------------------------------///
///  SwapChain   IMPLEMENTATION  ///
///------------------------------///
////////////////////////////////////
SwapChain::SwapChain(vk::raii::SurfaceKHR&& t_surface)
    : m_surface{ std::move(t_surface) }
{}

}   // namespace engine::renderer
