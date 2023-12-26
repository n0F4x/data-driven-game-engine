#include "Image.hpp"

#include <utility>

namespace engine::vulkan::vma {

Image::Image(
    VmaAllocator  t_allocator,
    vk::Image     t_image,
    VmaAllocation t_allocation
) noexcept
    : m_allocator{ t_allocator },
      m_image{ t_image },
      m_allocation{ t_allocation }
{}

Image::Image(Image&& t_other) noexcept
    : Image{ std::exchange(t_other.m_allocator, nullptr),
             std::exchange(t_other.m_image, nullptr),
             std::exchange(t_other.m_allocation, nullptr) }
{}

Image::~Image() noexcept
{
    reset();
}

auto Image::operator=(Image&& t_other) noexcept -> Image&
{
    if (this != &t_other) {
        reset();

        m_allocator  = std::exchange(t_other.m_allocator, nullptr);
        m_image      = std::exchange(t_other.m_image, nullptr);
        m_allocation = std::exchange(t_other.m_allocation, nullptr);
    }
    return *this;
}

auto Image::operator*() const noexcept -> vk::Image
{
    return m_image;
}

auto Image::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

auto Image::reset() noexcept -> void
{
    if (m_allocator) {
        vmaDestroyImage(
            m_allocator, static_cast<VkImage>(m_image), m_allocation
        );
    }
    m_allocation = nullptr;
    m_image      = nullptr;
    m_allocator  = nullptr;
}

}   // namespace engine::vulkan::vma
