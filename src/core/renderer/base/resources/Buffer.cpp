#include "Buffer.hpp"

auto core::renderer::base::Buffer::get() const -> vk::Buffer
{
    return m_buffer.get();
}

auto core::renderer::base::Buffer::size() const -> vk::DeviceSize
{
    return m_size;
}

auto core::renderer::base::Buffer::reset() -> void
{
    m_buffer.reset();
    m_size = 0;
}

core::renderer::base::Buffer::Buffer(
    vk::UniqueBuffer&&   buffer,
    const vk::DeviceSize buffer_size
) noexcept
    : m_buffer{ std::move(buffer) },
      m_size{ buffer_size }
{}
