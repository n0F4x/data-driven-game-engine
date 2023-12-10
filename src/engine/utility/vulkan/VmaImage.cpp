#include "VmaImage.hpp"

#include <utility>

namespace engine::vulkan {

VmaImage::VmaImage(
    VmaAllocator  t_allocator,
    vk::Image     t_image,
    VmaAllocation t_allocation
) noexcept
    : m_allocator{ t_allocator },
      m_image{ t_image },
      m_allocation{ t_allocation }
{}

VmaImage::VmaImage(VmaImage&& t_other) noexcept
    : VmaImage{ std::exchange(t_other.m_allocator, nullptr),
                std::exchange(t_other.m_image, nullptr),
                std::exchange(t_other.m_allocation, nullptr) }
{}

VmaImage::~VmaImage() noexcept
{
    destroy();
}

auto VmaImage::operator*() const noexcept -> vk::Image
{
    return m_image;
}

auto VmaImage::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

auto VmaImage::destroy() noexcept -> void
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

}   // namespace engine::vulkan
