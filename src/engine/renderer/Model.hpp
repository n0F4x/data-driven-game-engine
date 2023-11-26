#pragma once

#include <tuple>

#include <tl/optional.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "engine/graphics/Model.hpp"
#include "engine/utility/vulkan/VmaBuffer.hpp"

#include "Device.hpp"
#include "Mesh.hpp"

namespace engine::renderer {

class Model;

class StagingModel {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto upload_mesh(vk::CommandBuffer t_copy_command_buffer) const noexcept
        -> void;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend Model;

    ///*************///
    ///  Variables  ///
    ///*************///
    StagingMesh m_staging_mesh;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit StagingModel(StagingMesh&& t_staging_mesh) noexcept;
};

class Model {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///

    struct Mesh {
        using Primitive    = gfx::Model::Primitive;
        using UniformBlock = gfx::Model::Mesh::UniformBlock;

        std::vector<Primitive> primitives;

        vulkan::VmaBuffer        uniform_buffer;
        vk::DescriptorBufferInfo descriptor_buffer_info{};
        vk::DescriptorSet        descriptor_set{};
        void*                    mapped{};

        UniformBlock uniform_block;

        [[nodiscard]] static auto create(
            const Device&          t_device,
            std::vector<Primitive> t_primitives,
            const UniformBlock&    t_uniform_block
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
        const Device&     t_device,
        const gfx::Model& t_model
    ) noexcept -> tl::optional<std::tuple<StagingModel, Model>>;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    std::vector<Node> m_nodes;
    renderer::Mesh    m_mesh;

    ///******************************///
    ///  Constructors / Destructors  ///
    ///******************************///
    explicit Model(
        std::vector<Node>&& t_nodes,
        renderer::Mesh&&    t_mesh
    ) noexcept;
};

}   // namespace engine::renderer
