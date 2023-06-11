#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace engine::renderer {

class RendererImpl {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    RendererImpl(const vk::raii::Instance& t_instance,
                 vk::raii::SurfaceKHR&&    t_surface);

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::SurfaceKHR m_surface;
};

}   // namespace engine::renderer
