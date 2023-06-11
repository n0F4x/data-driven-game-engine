#include "Device.hpp"

namespace engine::renderer {

/////////////////////////////////
///---------------------------///
///  Device   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////
Device::Device(vk::raii::Instance&& t_instance, const vk::raii::SurfaceKHR&)
    : m_instance{ std::move(t_instance) }
{
}

}   // namespace engine::renderer
