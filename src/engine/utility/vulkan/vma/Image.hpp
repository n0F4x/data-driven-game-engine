#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace engine::vulkan::vma {

class Image {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Image(
        VmaAllocator  t_allocator,
        vk::Image     t_image,
        VmaAllocation t_allocation
    ) noexcept;
    Image(Image&&) noexcept;
    ~Image() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto               operator=(Image&&) noexcept -> Image&;
    [[nodiscard]] auto operator*() const noexcept -> vk::Image;

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
    vk::Image     m_image;
    VmaAllocation m_allocation;
};

}   // namespace engine::vulkan::vma
