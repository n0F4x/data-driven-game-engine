#include "Mesh.hpp"

namespace engine::renderer {

auto StagingMesh::upload(vk::CommandBuffer t_copy_command_buffer) const noexcept
    -> void
{
    vk::BufferCopy copy_region{ .size = m_vertex_buffer_size };
    t_copy_command_buffer.copyBuffer(
        *m_vertex_staging_buffer, m_vertex_buffer, 1, &copy_region
    );

    if (m_index_buffer_size > 0) {
        copy_region = { .size = m_index_buffer_size };
        t_copy_command_buffer.copyBuffer(
            *m_index_staging_buffer, m_index_buffer, 1, &copy_region
        );
    }
}

StagingMesh::StagingMesh(
    vulkan::VmaBuffer&& t_vertex_staging_buffer,
    vulkan::VmaBuffer&& t_index_staging_buffer,
    vk::Buffer          t_vertex_buffer,
    vk::Buffer          t_index_buffer,
    uint32_t            t_vertex_buffer_size,
    uint32_t            t_index_buffer_size
) noexcept
    : m_vertex_staging_buffer{ std::move(t_vertex_staging_buffer) },
      m_index_staging_buffer{ std::move(t_index_staging_buffer) },
      m_vertex_buffer{ t_vertex_buffer },
      m_index_buffer{ t_index_buffer },
      m_vertex_buffer_size{ t_vertex_buffer_size },
      m_index_buffer_size{ t_index_buffer_size }
{}

auto Mesh::bind(vk::CommandBuffer t_command_buffer) const noexcept -> void
{
    const std::array vertex_buffers{ *m_vertices.buffer };
    constexpr std::array<vk::DeviceSize, 1> offsets{ 0 };
    t_command_buffer.bindVertexBuffers(0, vertex_buffers, offsets);
    t_command_buffer.bindIndexBuffer(
        *m_indices.buffer, 0, vk::IndexType::eUint32
    );
}

Mesh::Mesh(Mesh::Vertices&& t_vertices, Mesh::Indices&& t_indices) noexcept
    : m_vertices{ std::move(t_vertices) },
      m_indices{ std::move(t_indices) }
{}

}   // namespace engine::renderer
