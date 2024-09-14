#pragma once

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

namespace core::renderer {

class Allocator;

}   // namespace core::renderer

namespace core::renderer {

class Image {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
     Image()             = default;
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
    friend Allocator;
    ///*************///
    ///  Variables  ///
    ///*************///
    vk::Image     m_image;
    VmaAllocation m_allocation{};
    VmaAllocator  m_allocator{};

    explicit
        Image(vk::Image image, VmaAllocation allocation, VmaAllocator allocator) noexcept;
};

}   // namespace core::renderer
