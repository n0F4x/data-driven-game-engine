#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace core::renderer {

class Image {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Image(
        vk::Image     t_image,
        VmaAllocation t_allocation,
        VmaAllocator  t_allocator
    ) noexcept;
    Image(const Image&) = delete;
    Image(Image&&) noexcept;
    ~Image() noexcept;

    ///-------------///
    ///  Operators  ///
    ///-------------///
    auto operator=(const Image&) -> Image& = delete;
    auto operator=(Image&&) noexcept -> Image&;
    [[nodiscard]]
    auto operator*() const noexcept -> vk::Image;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]]
    auto get() const noexcept -> vk::Image;

    [[nodiscard]]
    auto allocation() const noexcept -> VmaAllocation;
    auto reset() noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Image     m_image;
    VmaAllocation m_allocation;
    VmaAllocator  m_allocator;
};

}   // namespace core::renderer
