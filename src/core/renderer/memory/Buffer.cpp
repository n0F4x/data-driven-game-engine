#include "Buffer.hpp"

#include <utility>

namespace core::renderer {

Buffer::Buffer(
    const vk::Buffer    buffer,
    const VmaAllocation allocation,
    const VmaAllocator  allocator
) noexcept
    : m_buffer{ buffer },
      m_allocation{ allocation },
      m_allocator{ allocator }
{}

Buffer::Buffer(Buffer&& other) noexcept
    : Buffer{ std::exchange(other.m_buffer, nullptr),
              std::exchange(other.m_allocation, nullptr),
              std::exchange(other.m_allocator, nullptr) }
{}

Buffer::~Buffer() noexcept
{
    reset();
}

auto Buffer::operator=(Buffer&& other) noexcept -> Buffer&
{
    if (this != &other) {
        reset();

        m_allocator  = std::exchange(other.m_allocator, nullptr);
        m_buffer     = std::exchange(other.m_buffer, nullptr);
        m_allocation = std::exchange(other.m_allocation, nullptr);
    }
    return *this;
}

auto Buffer::operator*() const noexcept -> vk::Buffer
{
    return m_buffer;
}

auto Buffer::get() const noexcept -> vk::Buffer
{
    return m_buffer;
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
    m_allocation = nullptr;
    m_buffer     = nullptr;
    m_allocator  = nullptr;
}

}   // namespace core::renderer
