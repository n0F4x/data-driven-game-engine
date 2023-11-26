#pragma once

#include <tuple>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include "engine/utility/vulkan/VmaBuffer.hpp"

#include "Device.hpp"

namespace engine::renderer {

class Mesh;

class StagingMesh {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto upload(vk::CommandBuffer t_copy_command_buffer) const noexcept -> void;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Mesh;

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
    explicit StagingMesh(
        vulkan::VmaBuffer&& t_vertex_staging_buffer,
        vulkan::VmaBuffer&& t_index_staging_buffer,
        vk::Buffer          t_vertex_buffer,
        vk::Buffer          t_index_buffer,
        uint32_t            t_vertex_buffer_size,
        uint32_t            t_index_buffer_size
    ) noexcept;
};

class Mesh {
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
    ) noexcept -> tl::optional<std::tuple<StagingMesh, Mesh>>;

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
    explicit Mesh(Vertices&& t_vertices, Indices&& t_indices) noexcept;
};

}   // namespace engine::renderer

#include "Mesh.inl"
