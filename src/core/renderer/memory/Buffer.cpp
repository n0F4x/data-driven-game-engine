#include "Buffer.hpp"

#include <utility>

namespace core::renderer {

Buffer::Buffer(
    const vk::Buffer    t_buffer,
    const VmaAllocation t_allocation,
    const VmaAllocator  t_allocator
) noexcept
    : m_buffer{ t_buffer },
      m_allocation{ t_allocation },
      m_allocator{ t_allocator }
{}

Buffer::Buffer(Buffer&& t_other) noexcept
    : Buffer{ std::exchange(t_other.m_buffer, nullptr),
              std::exchange(t_other.m_allocation, nullptr),
              std::exchange(t_other.m_allocator, nullptr) }
{}

Buffer::~Buffer() noexcept
{
    reset();
}

auto Buffer::operator=(Buffer&& t_other) noexcept -> Buffer&
{
    if (this != &t_other) {
        reset();

        m_allocator  = std::exchange(t_other.m_allocator, nullptr);
        m_buffer     = std::exchange(t_other.m_buffer, nullptr);
        m_allocation = std::exchange(t_other.m_allocation, nullptr);
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
