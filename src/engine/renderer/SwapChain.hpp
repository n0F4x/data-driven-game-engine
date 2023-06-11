#pragma once

#include "Device.hpp"

namespace engine::renderer {

class SwapChain {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit SwapChain(vk::raii::SurfaceKHR&& t_surface);

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::SurfaceKHR m_surface;
};

}   // namespace engine::renderer
