#include "StagingMeshBuffer.hpp"

#include <vk_mem_alloc.h>

namespace core::renderer {

auto StagingMeshBuffer::upload(
    const Allocator&        t_allocator,
    const vk::CommandBuffer t_copy_command_buffer
) const -> MeshBuffer
{
    const vk::BufferCreateInfo vertex_buffer_create_info = {
        .size  = m_vertex_buffer_size,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    Buffer vertex_buffer{ t_allocator.create_buffer(vertex_buffer_create_info) };

    const vk::BufferCreateInfo index_buffer_create_info = {
        .size  = m_index_buffer_size,
        .usage = vk::BufferUsageFlagBits::eIndexBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    Buffer index_buffer{ t_allocator.create_buffer(index_buffer_create_info) };

    vk::BufferCopy copy_region{ .size = m_vertex_buffer_size };
    t_copy_command_buffer.copyBuffer(
        *m_vertex_staging_buffer, vertex_buffer.get(), 1, &copy_region
    );

    if (m_index_count > 0) {
        copy_region = { .size = m_index_buffer_size };
        t_copy_command_buffer.copyBuffer(
            *m_index_staging_buffer, index_buffer.get(), 1, &copy_region
        );
    }

    return MeshBuffer{
        MeshBuffer::Vertices{ .buffer = std::move(vertex_buffer) },
        MeshBuffer::Indices{ .count = m_index_count, .buffer = std::move(index_buffer) }
    };
}

StagingMeshBuffer::StagingMeshBuffer(
    Buffer&&       t_vertex_staging_buffer,
    Buffer&&       t_index_staging_buffer,
    const uint32_t t_vertex_buffer_size,
    const uint32_t t_index_buffer_size,
    const uint32_t t_index_count
) noexcept
    : m_vertex_staging_buffer{ std::move(t_vertex_staging_buffer) },
      m_index_staging_buffer{ std::move(t_index_staging_buffer) },
      m_vertex_buffer_size{ t_vertex_buffer_size },
      m_index_buffer_size{ t_index_buffer_size },
      m_index_count{ t_index_count }
{}

}   // namespace core::renderer
