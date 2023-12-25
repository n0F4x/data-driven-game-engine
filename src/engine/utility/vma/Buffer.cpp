#include "Buffer.hpp"

#include <utility>

namespace engine::vulkan::vma {

Buffer::Buffer(
    VmaAllocator  t_allocator,
    vk::Buffer    t_buffer,
    VmaAllocation t_allocation
) noexcept
    : m_allocator{ t_allocator },
      m_buffer{ t_buffer },
      m_allocation{ t_allocation }
{}

Buffer::Buffer(Buffer&& t_other) noexcept
    : Buffer{ std::exchange(t_other.m_allocator, nullptr),
              std::exchange(t_other.m_buffer, nullptr),
              std::exchange(t_other.m_allocation, nullptr) }
{}

Buffer::~Buffer() noexcept
{
    reset();
}

auto Buffer::operator=(Buffer&& t_other) noexcept -> Buffer&
{
    if (this != &t_other) {
        reset();

        std::swap(m_allocator, t_other.m_allocator);
        std::swap(m_buffer, t_other.m_buffer);
        std::swap(m_allocation, t_other.m_allocation);
    }
    return *this;
}

auto Buffer::operator*() const noexcept -> vk::Buffer
{
    return m_buffer;
}

auto Buffer::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

auto Buffer::reset() noexcept -> void
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

}   // namespace engine::vulkan::vma
