#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace core::renderer {

class Buffer {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Buffer() = default;
    explicit Buffer(
        vk::Buffer    t_buffer,
        VmaAllocation t_allocation,
        VmaAllocator  t_allocator
    ) noexcept;
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) noexcept;
    ~Buffer() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(const Buffer&) -> Buffer& = delete;
    auto operator=(Buffer&&) noexcept -> Buffer&;
    [[nodiscard]]
    auto operator*() const noexcept -> vk::Buffer;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> vk::Buffer;
    [[nodiscard]]
    auto allocation() const noexcept -> VmaAllocation;
    [[nodiscard]]
    auto allocator() const noexcept -> VmaAllocator;

    auto reset() noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Buffer    m_buffer;
    VmaAllocation m_allocation{};
    VmaAllocator  m_allocator{};
};

}   // namespace core::renderer
