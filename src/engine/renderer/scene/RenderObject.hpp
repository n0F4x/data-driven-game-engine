#pragma once

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "engine/gfx/Model.hpp"
#include "engine/renderer/Device.hpp"
#include "engine/utility/vulkan/vma/Buffer.hpp"

#include "MeshBuffer.hpp"

namespace engine::renderer {

class RenderObject {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    struct Mesh {
        using Primitive    = gfx::Model::Primitive;
        using UniformBlock = gfx::Model::Mesh::UniformBlock;

        std::vector<Primitive> primitives;

        vulkan::vma::Buffer uniform_buffer;
        vk::DescriptorSet   descriptor_set{};
        void*               mapped{};

        UniformBlock uniform_block;

        [[nodiscard]] static auto create(
            const Device&           t_device,
            vk::DescriptorSetLayout t_descriptor_set_layout,
            vk::DescriptorPool      t_descriptor_pool,
            std::vector<Primitive>  t_primitives,
            const UniformBlock&     t_uniform_block
        ) noexcept -> tl::optional<Mesh>;
    };

    struct Node {
        Node*              parent{};
        std::vector<Node>  children;
        tl::optional<Mesh> mesh;
        glm::mat4          matrix{ glm::identity<glm::mat4>() };
    };

    ///----------------///
    /// Static methods ///
    ///----------------///
    [[nodiscard]] static auto create(
        const Device&           t_device,
        vk::DescriptorSetLayout t_descriptor_set_layout,
        vk::DescriptorPool      t_descriptor_pool,
        const gfx::Model&       t_model,
        renderer::MeshBuffer&   t_mesh_buffer
    ) noexcept -> tl::optional<RenderObject>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto draw(
        vk::CommandBuffer  t_graphics_buffer,
        vk::PipelineLayout t_pipeline_layout
    ) const noexcept -> void;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    std::vector<Node>     m_nodes;
    renderer::MeshBuffer& m_mesh_buffer;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit RenderObject(
        std::vector<Node>&&   t_nodes,
        renderer::MeshBuffer& t_mesh
    ) noexcept;
};

}   // namespace engine::renderer
