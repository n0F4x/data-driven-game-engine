#pragma once

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "core/renderer/base/Allocator.hpp"
#include "core/renderer/model/MeshBuffer.hpp"
#include "core/utility/vma/Buffer.hpp"

namespace core::renderer {

class StagingMeshBuffer {
public:
    ///----------------///
    /// Static methods ///
    ///----------------///
    template <typename Vertex>
    [[nodiscard]] static auto create(
        const renderer::Allocator& t_allocator,
        std::span<const Vertex>    t_vertices,
        std::span<const uint32_t>  t_indices
    ) noexcept -> tl::optional<StagingMeshBuffer>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto upload(
        const renderer::Allocator& t_allocator,
        vk::CommandBuffer          t_copy_command_buffer
    ) const noexcept -> tl::optional<MeshBuffer>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    vma::Buffer m_vertex_staging_buffer;
    vma::Buffer m_index_staging_buffer;
    uint32_t    m_vertex_buffer_size;
    uint32_t    m_index_buffer_size;
    uint32_t    m_index_count;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit StagingMeshBuffer(
        vma::Buffer&& t_vertex_staging_buffer,
        vma::Buffer&& t_index_staging_buffer,
        uint32_t      t_vertex_buffer_size,
        uint32_t      t_index_buffer_size,
        uint32_t      t_index_count
    ) noexcept;
};

}   // namespace core::renderer

#include "StagingMeshBuffer.inl"
