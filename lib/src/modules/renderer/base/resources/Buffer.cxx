module;

#include <utility>

module modules.renderer.base.resources.Buffer;

import vulkan_hpp;

auto modules::renderer::base::Buffer::get() const -> vk::Buffer
{
    return m_buffer.get();
}

auto modules::renderer::base::Buffer::size() const -> vk::DeviceSize
{
    return m_size;
}

auto modules::renderer::base::Buffer::reset() -> void
{
    m_buffer.reset();
    m_size = 0;
}

modules::renderer::base::Buffer::Buffer(
    vk::UniqueBuffer&&   buffer,
    const vk::DeviceSize buffer_size
) noexcept
    : m_buffer{ std::move(buffer) },
      m_size{ buffer_size }
{}
