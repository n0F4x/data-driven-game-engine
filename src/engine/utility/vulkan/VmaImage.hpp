#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace engine::vulkan {

class VmaImage {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit VmaImage(
        VmaAllocator  t_allocator,
        vk::Image     t_image,
        VmaAllocation t_allocation
    ) noexcept;
    VmaImage(VmaImage&&) noexcept;
    ~VmaImage() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(VmaImage&&) noexcept -> VmaImage& = default;
    [[nodiscard]] auto operator*() const noexcept -> vk::Image;

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
    vk::Image     m_image;
    VmaAllocation m_allocation;
};

}   // namespace engine::vulkan
