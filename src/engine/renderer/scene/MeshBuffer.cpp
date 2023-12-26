#include "MeshBuffer.hpp"

namespace engine::renderer {

MeshBuffer::MeshBuffer(
    MeshBuffer::Vertices&& t_vertices,
    MeshBuffer::Indices&&  t_indices
) noexcept
    : m_vertices{ std::move(t_vertices) },
      m_indices{ std::move(t_indices) }
{}

auto MeshBuffer::bind(vk::CommandBuffer t_command_buffer) const noexcept -> void
{
    const std::array vertex_buffers{ *m_vertices.buffer };
    constexpr std::array<vk::DeviceSize, 1> offsets{ 0 };
    t_command_buffer.bindVertexBuffers(0, vertex_buffers, offsets);
    if (m_indices.count > 0) {
        t_command_buffer.bindIndexBuffer(
            *m_indices.buffer, 0, vk::IndexType::eUint32
        );
    }
}

}   // namespace engine::renderer
