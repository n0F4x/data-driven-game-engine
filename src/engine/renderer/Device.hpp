#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace engine::renderer {

class Device {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Device(vk::raii::Instance&&        t_instance,
                    const vk::raii::SurfaceKHR& t_surface);

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::Instance m_instance;
};

}   // namespace engine::renderer
