#include "FrameBuffer.hpp"

namespace engine::renderer::vulkan {

//////////////////////////////////////
///--------------------------------///
///  FrameBuffer   IMPLEMENTATION  ///
///--------------------------------///
//////////////////////////////////////

FrameBuffer::FrameBuffer(
    vk::Device      t_device,
    vk::Framebuffer t_frame_buffer
) noexcept
    : m_device{ t_device },
      m_frame_buffer{ t_frame_buffer }
{}

FrameBuffer::FrameBuffer(FrameBuffer&& t_other) noexcept
    : m_device{ t_other.m_device },
      m_frame_buffer{ t_other.m_frame_buffer }
{
    t_other.m_frame_buffer = nullptr;
}

FrameBuffer::~FrameBuffer() noexcept
{
    if (m_frame_buffer) {
        m_device.destroy(m_frame_buffer);
    }
}

auto FrameBuffer::operator*() const noexcept -> vk::Framebuffer
{
    return m_frame_buffer;
}

}   // namespace engine::renderer::vulkan
