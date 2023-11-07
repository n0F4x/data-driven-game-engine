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
    if (m_allocator) {
        vmaDestroyBuffer(
            m_allocator, static_cast<VkBuffer>(m_buffer), m_allocation
        );
    }
}

auto VmaBuffer::operator*() const noexcept -> vk::Buffer
{
    return m_buffer;
}

auto VmaBuffer::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

}   // namespace engine::vulkan
