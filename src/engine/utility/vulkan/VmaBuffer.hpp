#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace engine::vulkan {

class VmaBuffer {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit VmaBuffer(
        VmaAllocator  t_allocator,
        vk::Buffer    t_buffer,
        VmaAllocation t_allocation
    ) noexcept;
    VmaBuffer(VmaBuffer&&) noexcept;
    ~VmaBuffer() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(VmaBuffer&&) noexcept -> VmaBuffer&;
    [[nodiscard]] auto operator*() const noexcept -> vk::Buffer;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto allocation() const noexcept -> VmaAllocation;
    auto               destroy() noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    VmaAllocator  m_allocator;
    vk::Buffer    m_buffer;
    VmaAllocation m_allocation;
};

}   // namespace engine::vulkan
