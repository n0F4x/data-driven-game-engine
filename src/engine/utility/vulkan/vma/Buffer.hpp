#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace engine::vulkan::vma {

class Buffer {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Buffer(
        VmaAllocator  t_allocator,
        vk::Buffer    t_buffer,
        VmaAllocation t_allocation
    ) noexcept;
    Buffer(Buffer&&) noexcept;
    ~Buffer() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Buffer&&) noexcept -> Buffer&;
    [[nodiscard]] auto operator*() const noexcept -> vk::Buffer;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto allocation() const noexcept -> VmaAllocation;
    auto               reset() noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    VmaAllocator  m_allocator;
    vk::Buffer    m_buffer;
    VmaAllocation m_allocation;
};

}   // namespace engine::vulkan::vma
