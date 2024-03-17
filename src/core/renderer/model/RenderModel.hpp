#pragma once

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <entt/core/fwd.hpp>

#include "core/renderer/base/Allocator.hpp"
#include "core/utility/vma/Buffer.hpp"

#include "MeshBuffer.hpp"

namespace core::renderer {

class StagingModel;

class RenderModel {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Primitive {
        uint32_t first_index_index;
        uint32_t index_count;
        uint32_t vertex_count;
    };

    struct Mesh {
        struct UniformBlock {
            glm::mat4 matrix{ glm::identity<glm::mat4>() };
        };

        std::pmr::vector<Primitive> primitives;

        vma::Buffer             uniform_buffer;
        void*                   mapped{};
        vk::UniqueDescriptorSet descriptor_set;

        [[nodiscard]] auto upload(
            vk::Device              t_device,
            const Allocator&        t_allocator,
            vk::DescriptorSetLayout t_descriptor_set_layout,
            vk::DescriptorPool      t_descriptor_pool,
            const UniformBlock&     t_uniform_block
        ) -> bool;
    };

    struct Node {
        std::vector<Node>  children;
        tl::optional<Mesh> mesh;
        glm::mat4          matrix{ glm::identity<glm::mat4>() };

        [[nodiscard]] auto upload(
            vk::Device                 t_device,
            const renderer::Allocator& t_allocator,
            vk::DescriptorSetLayout    t_descriptor_set_layout,
            vk::DescriptorPool         t_descriptor_pool
        ) -> bool;

        auto draw(
            vk::CommandBuffer  t_graphics_buffer,
            vk::PipelineLayout t_pipeline_layout,
            const glm::mat4&   t_transform
        ) const noexcept -> void;
    };

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto draw(vk::CommandBuffer t_graphics_buffer, vk::PipelineLayout t_pipeline_layout)
        const noexcept -> void;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend StagingModel;

    ///*************///
    ///  Variables  ///
    ///*************///
    entt::id_type     m_id;
    std::vector<Node> m_nodes;
    MeshBuffer        m_mesh_buffer;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit RenderModel(std::vector<Node>&& t_nodes, MeshBuffer&& t_mesh_buffer) noexcept;
};

}   // namespace core::renderer
