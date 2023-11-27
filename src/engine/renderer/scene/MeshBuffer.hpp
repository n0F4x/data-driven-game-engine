#pragma once

#include <tuple>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "engine/renderer/Device.hpp"
#include "engine/utility/vulkan/VmaBuffer.hpp"

namespace engine::renderer {

class MeshBuffer;

class StagingMeshBuffer {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto upload(vk::CommandBuffer t_copy_command_buffer) const noexcept -> void;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend MeshBuffer;

    ///*************///
    ///  Variables  ///
    ///*************///
    vulkan::VmaBuffer m_vertex_staging_buffer;
    vulkan::VmaBuffer m_index_staging_buffer;
    vk::Buffer        m_vertex_buffer;
    vk::Buffer        m_index_buffer;
    uint32_t          m_vertex_buffer_size;
    uint32_t          m_index_buffer_size;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit StagingMeshBuffer(
        vulkan::VmaBuffer&& t_vertex_staging_buffer,
        vulkan::VmaBuffer&& t_index_staging_buffer,
        vk::Buffer          t_vertex_buffer,
        vk::Buffer          t_index_buffer,
        uint32_t            t_vertex_buffer_size,
        uint32_t            t_index_buffer_size
    ) noexcept;
};

class MeshBuffer {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Vertices {
        vulkan::VmaBuffer buffer;
    };

    struct Indices {
        uint32_t          count{};
        vulkan::VmaBuffer buffer;
    };

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    template <typename Vertex>
    [[nodiscard]] static auto create(
        const Device&             t_device,
        std::span<const Vertex>   t_vertices,
        std::span<const uint32_t> t_indices
    ) noexcept -> tl::optional<std::tuple<StagingMeshBuffer, MeshBuffer>>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto bind(vk::CommandBuffer t_command_buffer) const noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Vertices m_vertices;
    Indices  m_indices;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit MeshBuffer(Vertices&& t_vertices, Indices&& t_indices) noexcept;
};

}   // namespace engine::renderer

#include "MeshBuffer.inl"
