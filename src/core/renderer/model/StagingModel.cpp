#include "StagingModel.hpp"

namespace core::renderer {

auto StagingModel::nodes() const noexcept -> const std::vector<Node>&
{
    return m_nodes;
}

auto StagingModel::upload(
    const vk::Device              t_device,
    const Allocator&              t_allocator,
    const vk::CommandBuffer       t_copy_command_buffer,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool
) && -> RenderModel
{
    RenderModel model{ std::move(m_nodes),
                       m_staging_mesh_buffer.upload(t_allocator, t_copy_command_buffer) };

    for (auto& node : model.m_nodes) {
        node.upload(t_device, t_allocator, t_descriptor_set_layout, t_descriptor_pool);
    }

    return model;
}

StagingModel::StagingModel(
    StagingMeshBuffer&& t_staging_mesh_buffer,
    std::vector<Node>&& t_nodes
) noexcept
    : m_staging_mesh_buffer{ std::move(t_staging_mesh_buffer) },
      m_nodes{ std::move(t_nodes) }
{}

}   // namespace core::renderer
