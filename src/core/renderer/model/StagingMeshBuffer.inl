#include <vk_mem_alloc.h>

namespace core::renderer {

template <typename Vertex>
auto StagingMeshBuffer::create(
    const Allocator&                t_allocator,
    const std::span<const Vertex>   t_vertices,
    const std::span<const uint32_t> t_indices
) noexcept -> tl::optional<StagingMeshBuffer>
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
    constexpr VmaAllocationCreateInfo staging_allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    auto vertex_staging_buffer{ t_allocator.create_buffer(
        vertex_staging_buffer_create_info,
        staging_allocation_create_info,
        t_vertices.data()
    ) };
    if (!vertex_staging_buffer) {
        return tl::nullopt;
    }
    auto index_staging_buffer{ t_allocator.create_buffer(
        index_staging_buffer_create_info, staging_allocation_create_info, t_indices.data()
    ) };
    if (!index_staging_buffer) {
        return tl::nullopt;
    }

    return StagingMeshBuffer{ std::move(std::get<vma::Buffer>(*vertex_staging_buffer)),
                              std::move(std::get<vma::Buffer>(*index_staging_buffer)),
                              vertex_buffer_size,
                              index_buffer_size,
                              static_cast<uint32_t>(t_indices.size()) };
}

}   // namespace core::renderer
