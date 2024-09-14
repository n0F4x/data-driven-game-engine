#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace core::renderer {

class Allocator;

class Buffer {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
     Buffer()              = default;
     Buffer(const Buffer&) = delete;
     Buffer(Buffer&&) noexcept;
    ~Buffer() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(const Buffer&) -> Buffer& = delete;
    auto operator=(Buffer&&) noexcept -> Buffer&;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> vk::Buffer;

    [[nodiscard]]
    auto size_bytes() const noexcept -> vk::DeviceSize;

    auto reset() noexcept -> void;

protected:
    [[nodiscard]]
    auto allocation() const noexcept -> VmaAllocation;
    [[nodiscard]]
    auto allocator() const noexcept -> VmaAllocator;

private:
    friend Allocator;

    ///*************///
    ///  Variables  ///
    ///*************///
    VmaAllocator   m_allocator{};
    VmaAllocation  m_allocation{};
    vk::Buffer     m_buffer;
    vk::DeviceSize m_buffer_size{};

    explicit Buffer(
        VmaAllocator   allocator,
        VmaAllocation  allocation,
        vk::Buffer     buffer,
        vk::DeviceSize buffer_size
    ) noexcept;
};

}   // namespace core::renderer
