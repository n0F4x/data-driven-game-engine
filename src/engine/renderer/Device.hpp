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

    auto physical_device() -> const vk::raii::PhysicalDevice&;
    auto device() -> const vk::raii::Device&;

    auto graphics_queue() -> const vk::raii::Queue&;
    auto present_queue() -> const vk::raii::Queue&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::Instance       m_instance;
    vk::raii::PhysicalDevice m_physical_device;
    vk::raii::Device         m_device;
    vk::raii::Queue          m_graphics_queue;
    vk::raii::Queue          m_present_queue;
};

}   // namespace engine::renderer
