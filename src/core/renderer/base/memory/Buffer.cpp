#include "Buffer.hpp"

#include <utility>

namespace core::renderer::base {

Buffer::Buffer(Buffer&& other) noexcept
    : Buffer{ std::exchange(other.m_allocator, nullptr),
              std::exchange(other.m_allocation, nullptr),
              std::exchange(other.m_buffer, nullptr),
              std::exchange(other.m_buffer_size, 0) }
{}

Buffer::~Buffer() noexcept
{
    reset();
}

auto Buffer::operator=(Buffer&& other) noexcept -> Buffer&
{
    if (this != &other) {
        reset();

        m_allocator   = std::exchange(other.m_allocator, nullptr);
        m_allocation  = std::exchange(other.m_allocation, nullptr);
        m_buffer      = std::exchange(other.m_buffer, nullptr);
        m_buffer_size = std::exchange(other.m_buffer_size, 0);
    }
    return *this;
}

auto Buffer::get() const noexcept -> vk::Buffer
{
    return m_buffer;
}

auto Buffer::size_bytes() const noexcept -> vk::DeviceSize
{
    return m_buffer_size;
}

auto Buffer::allocation() const noexcept -> VmaAllocation
{
    return m_allocation;
}

auto Buffer::allocator() const noexcept -> VmaAllocator
{
    return m_allocator;
}

auto Buffer::reset() noexcept -> void
{
    if (m_allocator != nullptr) {
        vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
    }

    m_buffer_size = 0;
    m_buffer      = nullptr;
    m_allocation  = nullptr;
    m_allocator   = nullptr;
}

Buffer::Buffer(
    const VmaAllocator   allocator,
    const VmaAllocation  allocation,
    const vk::Buffer     buffer,
    const vk::DeviceSize buffer_size
) noexcept
    : m_allocator{ allocator },
      m_allocation{ allocation },
      m_buffer{ buffer },
      m_buffer_size{ buffer_size }
{}

}   // namespace core::renderer
