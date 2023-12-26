#pragma once

#include <span>
#include <string_view>
#include <utility>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include "engine/utility/vma/Allocator.hpp"
#include "engine/utility/vma/Buffer.hpp"

#include "Instance.hpp"

namespace engine::renderer {

class Device {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct CreateInfo {
        const void*                  next{};
        std::span<const std::string> extensions{};
        vk::PhysicalDeviceFeatures   features{};
    };

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        const Instance&    t_instance,
        vk::SurfaceKHR     t_surface,
        vk::PhysicalDevice t_physical_device,
        const CreateInfo&  t_create_info
    ) noexcept -> tl::optional<Device>;

    [[nodiscard]] static auto create_default(
        const Instance&    t_instance,
        vk::SurfaceKHR     t_surface,
        vk::PhysicalDevice t_physical_device
    ) noexcept -> tl::optional<Device>;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    [[nodiscard]] auto operator*() const noexcept -> vk::Device;
    [[nodiscard]] auto operator->() const noexcept -> const vk::Device*;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto physical_device() const noexcept -> vk::PhysicalDevice;
    [[nodiscard]] auto info() const noexcept -> const CreateInfo&;
    [[nodiscard]] auto allocator() const noexcept -> VmaAllocator;
    [[nodiscard]] auto graphics_queue_family_index() const noexcept -> uint32_t;
    [[nodiscard]] auto graphics_queue() const noexcept -> vk::Queue;
    [[nodiscard]] auto compute_queue_family_index() const noexcept -> uint32_t;
    [[nodiscard]] auto compute_queue() const noexcept -> vk::Queue;
    [[nodiscard]] auto transfer_queue_family_index() const noexcept -> uint32_t;
    [[nodiscard]] auto transfer_queue() const noexcept -> vk::Queue;

    [[nodiscard]] auto create_buffer(
        const vk::BufferCreateInfo&    t_buffer_create_info,
        const VmaAllocationCreateInfo& t_allocation_create_info,
        const void*                    t_data = nullptr
    ) const noexcept -> tl::optional<std::pair<vma::Buffer, VmaAllocationInfo>>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::PhysicalDevice m_physical_device;
    CreateInfo         m_info;
    vk::UniqueDevice   m_device;
    vma::Allocator     m_allocator;
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
        const CreateInfo&  t_info,
        vk::UniqueDevice&& t_device,
        VmaAllocator       t_allocator,
        uint32_t           t_graphics_family_index,
        vk::Queue          t_graphics_queue,
        uint32_t           t_compute_queue_family_index,
        vk::Queue          t_compute_queue,
        uint32_t           t_transfer_queue_family_index,
        vk::Queue          t_transfer_queue
    ) noexcept;
};

}   // namespace engine::renderer
