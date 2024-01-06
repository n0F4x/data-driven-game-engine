#include "Model.hpp"

#include <spdlog/spdlog.h>

namespace engine::scene {

[[nodiscard]] auto create_descriptor_set(
    const vk::Device         t_device,
    const vk::DescriptorSetLayout  t_descriptor_set_layout,
    const vk::DescriptorPool t_descriptor_pool,
    const vk::DescriptorBufferInfo t_descriptor_buffer_info
) -> vk::UniqueDescriptorSet
{
    const vk::DescriptorSetAllocateInfo descriptor_set_allocate_info{
        .descriptorPool     = t_descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = &t_descriptor_set_layout,
    };
    auto descriptor_sets{
        t_device.allocateDescriptorSetsUnique(descriptor_set_allocate_info)
    };

    const vk::WriteDescriptorSet write_descriptor_set{
        .dstSet          = descriptor_sets.front().get(),
        .dstBinding      = 0,
        .descriptorCount = 1u,
        .descriptorType  = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo     = &t_descriptor_buffer_info,
    };

    t_device.updateDescriptorSets(1, &write_descriptor_set, 0, nullptr);

    return std::move(descriptor_sets.front());
}

auto Model::Mesh::upload(
    const vk::Device              t_device,
    const renderer::Allocator&    t_allocator,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool,
    const UniformBlock&           t_uniform_block
) noexcept -> bool
{
    const vk::BufferCreateInfo buffer_create_info = {
        .size  = sizeof(t_uniform_block),
        .usage = vk::BufferUsageFlagBits::eUniformBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    constexpr VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    return t_allocator
        .create_buffer(
            buffer_create_info, allocation_create_info, &t_uniform_block
        )
        .transform([&](std::pair<vma::Buffer, VmaAllocationInfo>&& result) {
            const vk::DescriptorBufferInfo descriptor_buffer_info{
                .buffer = *result.first,
                .offset = 0,
                .range  = sizeof(UniformBlock)
            };

            uniform_buffer = std::move(result.first);
            mapped         = result.second.pMappedData;
            descriptor_set = create_descriptor_set(
                t_device,
                t_descriptor_set_layout,
                t_descriptor_pool,
                descriptor_buffer_info
            );
            return true;
        })
        .value_or(false);
}

auto Model::Node::upload(
    const vk::Device              t_device,
    const renderer::Allocator&    t_allocator,
    const vk::DescriptorSetLayout t_descriptor_set_layout,
    const vk::DescriptorPool      t_descriptor_pool
) noexcept -> bool
{
    if (mesh.transform([&](Mesh& t_mesh) {
                return t_mesh.upload(
                    t_device,
                    t_allocator,
                    t_descriptor_set_layout,
                    t_descriptor_pool,
                    Mesh::UniformBlock{ .matrix = matrix }
                );
            }
        ).value_or(true))
    {
        for (auto& child : children) {
            if (!child.upload(
                    t_device,
                    t_allocator,
                    t_descriptor_set_layout,
                    t_descriptor_pool
                ))
            {
                return false;
            }
        }
    }
    return true;
}

auto Model::Node::draw(
    const vk::CommandBuffer  t_graphics_buffer,
    const vk::PipelineLayout t_pipeline_layout,
    const glm::mat4&         t_transform
) const noexcept -> void
{
    const auto global_matrix{ t_transform * matrix };

    if (mesh) {
        const Mesh::UniformBlock uniform_block{ .matrix = global_matrix };
        memcpy(mesh->mapped, &uniform_block, sizeof(Mesh::UniformBlock));

        for (const auto& [first_index_index, index_count, vertex_count] :
             mesh->primitives)
        {
            t_graphics_buffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                t_pipeline_layout,
                0,
                *mesh->descriptor_set,
                nullptr
            );

            if (index_count > 0) {
                t_graphics_buffer.drawIndexed(
                    index_count, 1, first_index_index, 0, 0
                );
            }
            else {
                t_graphics_buffer.draw(vertex_count, 1, 0, 0);
            }
        }
    }

    for (const auto& child : children) {
        child.draw(t_graphics_buffer, t_pipeline_layout, global_matrix);
    }
}

auto Model::draw(
    const vk::CommandBuffer  t_graphics_buffer,
    const vk::PipelineLayout t_pipeline_layout
) const noexcept -> void
{
    m_mesh_buffer.bind(t_graphics_buffer);

    for (const auto& node : m_nodes) {
        node.draw(
            t_graphics_buffer, t_pipeline_layout, glm::identity<glm::mat4>()
        );
    }
}

Model::Model(std::vector<Node>&& t_nodes, MeshBuffer&& t_mesh_buffer) noexcept
    : m_nodes{ std::move(t_nodes) },
      m_mesh_buffer{ std::move(t_mesh_buffer) }
{}

}   // namespace engine::scene
