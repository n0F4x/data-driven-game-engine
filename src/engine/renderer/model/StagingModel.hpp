#pragma once

#include <vector>

#include "engine/renderer/base/Allocator.hpp"

#include "Model.hpp"
#include "StagingMeshBuffer.hpp"

namespace engine::scene {

class ModelFactory;

class StagingModel {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    using Vertex    = Model::Vertex;
    using Primitive = Model::Primitive;
    using Mesh      = Model::Mesh;
    using Node      = Model::Node;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto nodes() const noexcept -> const std::vector<Node>&;

    [[nodiscard]] auto upload(
        vk::Device                 t_device,
        const renderer::Allocator& t_allocator,
        vk::CommandBuffer          t_copy_command_buffer,
        vk::DescriptorSetLayout    t_descriptor_set_layout,
        vk::DescriptorPool         t_descriptor_pool
    ) && noexcept -> tl::optional<Model>;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend ModelFactory;

    ///*************///
    ///  Variables  ///
    ///*************///
    StagingMeshBuffer m_staging_mesh_buffer;
    std::vector<Node> m_nodes;

    explicit StagingModel(
        StagingMeshBuffer&& t_staging_mesh_buffer,
        std::vector<Node>&& t_nodes
    ) noexcept;
};

}   // namespace engine::scene
