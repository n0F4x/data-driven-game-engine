#include "Image.hpp"

#include <utility>

namespace core::renderer::base {

Image::Image(
    const vk::Image     image,
    const VmaAllocation allocation,
    const VmaAllocator  allocator
) noexcept
    : m_image{ image },
      m_allocation{ allocation },
      m_allocator{ allocator }
{}

Image::Image(Image&& other) noexcept
    : Image{ std::exchange(other.m_image, nullptr),
             std::exchange(other.m_allocation, nullptr),
             std::exchange(other.m_allocator, nullptr) }
{}

Image::~Image() noexcept
{
    reset();
}

auto Image::operator=(Image&& other) noexcept -> Image&
{
    if (this != &other) {
        reset();

        m_allocator  = std::exchange(other.m_allocator, nullptr);
        m_image      = std::exchange(other.m_image, nullptr);
        m_allocation = std::exchange(other.m_allocation, nullptr);
    }
    return *this;
}

auto Image::operator*() const noexcept -> vk::Image
{
    return m_image;
}

auto Image::get() const noexcept -> vk::Image
{
    return m_image;
}

auto Image::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

auto Image::reset() noexcept -> void
{
    if (m_allocator != nullptr) {
        vmaDestroyImage(m_allocator, m_image, m_allocation);
    }
    m_allocation = nullptr;
    m_image      = nullptr;
    m_allocator  = nullptr;
}

}   // namespace core::renderer
