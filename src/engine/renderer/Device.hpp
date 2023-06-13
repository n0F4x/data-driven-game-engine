#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace engine::renderer {

class Device {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Device(
        vk::raii::Instance&& t_instance, const vk::raii::SurfaceKHR& t_surface
    );

    auto physical_device() const -> const vk::raii::PhysicalDevice&;
    auto device() const -> const vk::raii::Device&;

    auto graphics_queue_family() const -> uint32_t;
    auto present_queue_family() const -> uint32_t;

    auto graphics_queue() const -> const vk::raii::Queue&;
    auto present_queue() const -> const vk::raii::Queue&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    vk::raii::Instance       m_instance;
    vk::raii::PhysicalDevice m_physical_device;
    vk::raii::Device         m_device;
    uint32_t                 m_graphics_queue_family;
    uint32_t                 m_present_queue_family;
    vk::raii::Queue          m_graphics_queue;
    vk::raii::Queue          m_present_queue;
};

}   // namespace engine::renderer
