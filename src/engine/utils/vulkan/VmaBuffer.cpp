#include "VmaBuffer.hpp"

#include <utility>

namespace engine::vulkan {

VmaBuffer::VmaBuffer(
    VmaAllocator  t_allocator,
    vk::Buffer    t_buffer,
    VmaAllocation t_allocation
) noexcept
    : m_allocator{ t_allocator },
      m_buffer{ t_buffer },
      m_allocation{ t_allocation }
{}

VmaBuffer::VmaBuffer(VmaBuffer&& t_other) noexcept
    : VmaBuffer{ std::exchange(t_other.m_allocator, nullptr),
                 std::exchange(t_other.m_buffer, nullptr),
                 std::exchange(t_other.m_allocation, nullptr) }
{}

VmaBuffer::~VmaBuffer() noexcept
{
    destroy();
}

auto VmaBuffer::operator=(VmaBuffer&& t_other) noexcept -> VmaBuffer&
{
    if (this != &t_other) {
        destroy();

        std::swap(m_allocator, t_other.m_allocator);
        std::swap(m_buffer, t_other.m_buffer);
        std::swap(m_allocation, t_other.m_allocation);
    }
    return *this;
}

auto VmaBuffer::operator*() const noexcept -> vk::Buffer
{
    return m_buffer;
}

auto VmaBuffer::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

auto VmaBuffer::destroy() noexcept -> void
{
    if (m_allocator) {
        vmaDestroyBuffer(
            m_allocator, static_cast<VkBuffer>(m_buffer), m_allocation
        );
    }
    m_allocation = nullptr;
    m_buffer     = nullptr;
    m_allocator  = nullptr;
}

}   // namespace engine::vulkan
