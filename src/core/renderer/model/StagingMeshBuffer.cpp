#include "StagingMeshBuffer.hpp"

#include <vk_mem_alloc.h>

namespace core::renderer {

auto StagingMeshBuffer::upload(
    const renderer::Allocator& t_allocator,
    const vk::CommandBuffer    t_copy_command_buffer
) const noexcept -> tl::optional<MeshBuffer>
{
    const vk::BufferCreateInfo vertex_buffer_create_info = {
        .size  = m_vertex_buffer_size,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    constexpr VmaAllocationCreateInfo vertex_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    auto vertex_buffer{ t_allocator.create_buffer(
        vertex_buffer_create_info, vertex_allocation_create_info
    ) };
    if (!vertex_buffer) {
        return tl::nullopt;
    }

    const vk::BufferCreateInfo index_buffer_create_info = {
        .size  = m_index_buffer_size,
        .usage = vk::BufferUsageFlagBits::eIndexBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    constexpr VmaAllocationCreateInfo index_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    auto index_buffer{
        t_allocator.create_buffer(index_buffer_create_info, index_allocation_create_info)
    };
    if (!index_buffer) {
        return tl::nullopt;
    }


    vk::BufferCopy copy_region{ .size = m_vertex_buffer_size };
    t_copy_command_buffer.copyBuffer(
        *m_vertex_staging_buffer, *std::get<vma::Buffer>(*vertex_buffer), 1, &copy_region
    );

    if (m_index_count > 0) {
        copy_region = { .size = m_index_buffer_size };
        t_copy_command_buffer.copyBuffer(
            *m_index_staging_buffer, *std::get<vma::Buffer>(*index_buffer), 1, &copy_region
        );
    }

    return MeshBuffer{
        MeshBuffer::Vertices{ .buffer = std::move(std::get<vma::Buffer>(*vertex_buffer)) },
        MeshBuffer::Indices{ .count  = m_index_count,
                             .buffer = std::move(std::get<vma::Buffer>(*index_buffer)) }
    };
}

StagingMeshBuffer::StagingMeshBuffer(
    vma::Buffer&&  t_vertex_staging_buffer,
    vma::Buffer&&  t_index_staging_buffer,
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
