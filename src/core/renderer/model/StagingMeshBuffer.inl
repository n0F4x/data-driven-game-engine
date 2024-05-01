#include <vk_mem_alloc.h>

namespace core::renderer {

template <typename Vertex>
auto StagingMeshBuffer::create(
    const Allocator&                t_allocator,
    const std::span<const Vertex>   t_vertices,
    const std::span<const uint32_t> t_indices
) -> StagingMeshBuffer
{
    const auto vertex_buffer_size = static_cast<uint32_t>(t_vertices.size_bytes());
    const auto index_buffer_size  = static_cast<uint32_t>(t_indices.size_bytes());

    const vk::BufferCreateInfo vertex_staging_buffer_create_info = {
        .size  = vertex_buffer_size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };
    const vk::BufferCreateInfo index_staging_buffer_create_info = {
        .size  = index_buffer_size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };

    return StagingMeshBuffer{ t_allocator.create_mapped_buffer(
                                  vertex_staging_buffer_create_info, t_vertices.data()
                              ),
                              t_allocator.create_mapped_buffer(
                                  index_staging_buffer_create_info, t_indices.data()
                              ),
                              vertex_buffer_size,
                              index_buffer_size,
                              static_cast<uint32_t>(t_indices.size()) };
}

}   // namespace core::renderer
