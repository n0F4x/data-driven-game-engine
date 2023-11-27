namespace engine::renderer {

template <typename Vertex>
auto MeshBuffer::create(
    const Device&             t_device,
    std::span<const Vertex>   t_vertices,
    std::span<const uint32_t> t_indices
) noexcept -> tl::optional<std::tuple<StagingMeshBuffer, MeshBuffer>>
{
    auto vertex_buffer_size = static_cast<uint32_t>(t_vertices.size_bytes());
    auto index_buffer_size  = static_cast<uint32_t>(t_indices.size_bytes());

    const vk::BufferCreateInfo staging_buffer_create_info = {
        .size  = vertex_buffer_size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
    };
    constexpr VmaAllocationCreateInfo staging_allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    auto vertex_staging_buffer{ t_device.create_buffer(
        staging_buffer_create_info,
        staging_allocation_create_info,
        t_vertices.data()
    ) };
    if (!vertex_staging_buffer) {
        return tl::nullopt;
    }
    auto index_staging_buffer{ t_device.create_buffer(
        staging_buffer_create_info,
        staging_allocation_create_info,
        t_indices.data()
    ) };
    if (!index_staging_buffer) {
        return tl::nullopt;
    }


    const vk::BufferCreateInfo vertex_buffer_create_info = {
        .size  = vertex_buffer_size,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    constexpr VmaAllocationCreateInfo vertex_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    auto vertex_buffer{ t_device.create_buffer(
        vertex_buffer_create_info, vertex_allocation_create_info
    ) };
    if (!vertex_buffer) {
        return tl::nullopt;
    }

    const vk::BufferCreateInfo index_buffer_create_info = {
        .size  = index_buffer_size,
        .usage = vk::BufferUsageFlagBits::eIndexBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    constexpr VmaAllocationCreateInfo index_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    auto index_buffer{ t_device.create_buffer(
        index_buffer_create_info, index_allocation_create_info
    ) };
    if (!index_buffer) {
        return tl::nullopt;
    }


    return std::make_tuple(
        StagingMeshBuffer{
            std::move(std::get<vulkan::VmaBuffer>(*vertex_staging_buffer)),
            std::move(std::get<vulkan::VmaBuffer>(*index_staging_buffer)),
            *std::get<vulkan::VmaBuffer>(*vertex_buffer),
            *std::get<vulkan::VmaBuffer>(*index_buffer),
            vertex_buffer_size,
            index_buffer_size
    },
        MeshBuffer{ Vertices{ .buffer = std::move(
                                  std::get<vulkan::VmaBuffer>(*vertex_buffer)
                              ) },
                    Indices{ .count  = static_cast<uint32_t>(t_indices.size()),
                             .buffer = std::move(
                                 std::get<vulkan::VmaBuffer>(*index_buffer)
                             ) } }
    );
}

}   // namespace engine::renderer
