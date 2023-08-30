#pragma once

#include <optional>
#include <span>

#include <vulkan/vulkan.hpp>

#include "vulkan/Device.hpp"

namespace engine::renderer {

class Device {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Config {
        std::span<const char* const> extensions;
        vk::PhysicalDeviceFeatures   features{};
        const void*                  next;
    };

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        vk::Instance       t_instance,
        vk::SurfaceKHR     t_surface,
        vk::PhysicalDevice t_physical_device,
        const Config&      t_config
    ) noexcept -> std::optional<Device>;

    [[nodiscard]] static auto create_default(
        vk::Instance       t_instance,
        vk::SurfaceKHR     t_surface,
        vk::PhysicalDevice t_physical_device
    ) noexcept -> std::optional<Device>;

    [[nodiscard]] static auto default_extensions() noexcept
        -> std::span<const char* const>;

    [[nodiscard]] static auto adequate(
        vk::PhysicalDevice t_physical_device,
        vk::SurfaceKHR     t_surface
    ) noexcept -> bool;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]] auto operator*() const noexcept -> vk::Device;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Device*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto physical_device() const noexcept -> vk::PhysicalDevice;
    [[nodiscard]] auto graphics_queue_family_index() const noexcept -> uint32_t;
    [[nodiscard]] auto graphics_queue() const noexcept -> vk::Queue;
    [[nodiscard]] auto compute_queue_family_index() const noexcept -> uint32_t;
    [[nodiscard]] auto compute_queue() const noexcept -> vk::Queue;
    [[nodiscard]] auto transfer_queue_family_index() const noexcept -> uint32_t;
    [[nodiscard]] auto transfer_queue() const noexcept -> vk::Queue;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::PhysicalDevice m_physical_device;
    vulkan::Device     m_device;
    uint32_t           m_graphics_queue_family_index;
    vk::Queue          m_graphics_queue;
    uint32_t           m_compute_queue_family_index;
    vk::Queue          m_compute_queue;
    uint32_t           m_transfer_queue_family_index;
    vk::Queue          m_transfer_queue;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Device(
        vk::PhysicalDevice t_physical_device,
        vk::Device         t_device,
        uint32_t           t_graphics_family_index,
        vk::Queue          t_graphics_queue,
        uint32_t           t_compute_queue_family_index,
        vk::Queue          t_compute_queue,
        uint32_t           t_transfer_queue_family_index,
        vk::Queue          t_transfer_queue
    ) noexcept;
};

}   // namespace engine::renderer
