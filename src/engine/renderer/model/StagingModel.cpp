#include "StagingModel.hpp"

namespace engine::renderer {

auto StagingModel::nodes() const noexcept -> const std::vector<Node>&
{
    return m_nodes;
}

auto StagingModel::upload(
    const vk::Device              t_device,
    const renderer::Allocator&    t_allocator,
    const vk::CommandBuffer       t_copy_command_buffer,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool
) && noexcept -> tl::optional<RenderModel>
{
    return m_staging_mesh_buffer.upload(t_allocator, t_copy_command_buffer)
        .and_then([&](MeshBuffer&& mesh_buffer) -> tl::optional<RenderModel> {
            RenderModel model{ std::move(m_nodes), std::move(mesh_buffer) };

            for (auto& node : model.m_nodes) {
                if (!node.upload(
                        t_device, t_allocator, t_descriptor_set_layout, t_descriptor_pool
                    ))
                {
                    return tl::nullopt;
                }
            }

            return model;
        });
}

StagingModel::StagingModel(
    StagingMeshBuffer&& t_staging_mesh_buffer,
    std::vector<Node>&& t_nodes
) noexcept
    : m_staging_mesh_buffer{ std::move(t_staging_mesh_buffer) },
      m_nodes{ std::move(t_nodes) }
{}

}   // namespace engine::renderer
