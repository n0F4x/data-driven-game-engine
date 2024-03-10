#pragma once

#include <vector>

#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/model/RenderModel.hpp"

#include "StagingMeshBuffer.hpp"

namespace engine::renderer {

class ModelLoader;

class StagingModel {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    using Primitive = RenderModel::Primitive;
    using Mesh      = RenderModel::Mesh;
    using Node      = RenderModel::Node;

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
    ) && -> tl::optional<RenderModel>;

private:
    ///******************///
    ///  Friend Classes  ///
    ///******************///
    friend ModelLoader;

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

}   // namespace engine::renderer
