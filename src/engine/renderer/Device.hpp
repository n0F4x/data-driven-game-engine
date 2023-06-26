#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace engine::renderer {

class Device {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Device(
        vk::raii::Instance&&        t_instance,
        const vk::raii::SurfaceKHR& t_surface
    );

    [[nodiscard]] auto physical_device() const noexcept
        -> const vk::raii::PhysicalDevice&;
    [[nodiscard]] auto device() const noexcept -> const vk::raii::Device&;

    [[nodiscard]] auto graphics_queue_family() const noexcept -> uint32_t;
    [[nodiscard]] auto present_queue_family() const noexcept -> uint32_t;

    [[nodiscard]] auto graphics_queue() const noexcept
        -> const vk::raii::Queue&;
    [[nodiscard]] auto present_queue() const noexcept -> const vk::raii::Queue&;

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
