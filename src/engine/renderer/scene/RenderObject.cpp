#include "RenderObject.hpp"

namespace engine::renderer {

auto RenderObject::Mesh::create(
    const Device&          t_device,
    std::vector<Primitive> t_primitives,
    const UniformBlock&    t_uniform_block
) noexcept -> tl::optional<Mesh>
{
    const vk::BufferCreateInfo buffer_create_info = {
        .size  = sizeof(t_uniform_block),
        .usage = vk::BufferUsageFlagBits::eUniformBuffer
               | vk::BufferUsageFlagBits::eTransferDst
    };
    const VmaAllocationCreateInfo allocation_create_info = {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
               | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    return t_device
        .create_buffer(
            buffer_create_info, allocation_create_info, &t_uniform_block
        )
        .transform([&](std::tuple<vulkan::VmaBuffer, VmaAllocationInfo>&& result
                   ) {
            auto [_uniform_buffer, allocation_info]{ std::move(result) };
            auto buffer{ *_uniform_buffer };
            return Mesh{
                .primitives     = std::move(t_primitives),
                .uniform_buffer = std::move(_uniform_buffer),
                .descriptor_buffer_info =
                    vk::DescriptorBufferInfo{.buffer = buffer,
                                             .offset = 0,
                                             .range  = sizeof(uniform_block)},
                .mapped        = allocation_info.pMappedData,
                .uniform_block = t_uniform_block,
            };
        });
}

[[nodiscard]] static auto create_node(
    const Device&           t_device,
    const gfx::Model::Node& t_node,
    RenderObject::Node*     t_parent
) noexcept -> tl::optional<RenderObject::Node>
{
    tl::optional<RenderObject::Mesh> mesh;
    if (auto src_mesh = t_node.mesh; src_mesh.has_value()) {
        mesh = { RenderObject::Mesh::create(
            t_device, src_mesh->primitives, src_mesh->uniform_block
        ) };
        if (!mesh) {
            return tl::nullopt;
        }
    }

    RenderObject::Node node{
        .parent = t_parent,
        .mesh   = std::move(mesh),
        .matrix = t_node.matrix,
    };

    node.children.reserve(t_node.children.size());
    for (const auto& src_node : t_node.children) {
        auto child = create_node(t_device, src_node, &node);
        if (!child) {
            return tl::nullopt;
        }
        node.children.push_back(std::move(*child));
    }

    return node;
}

auto RenderObject::create(
    const Device&         t_device,
    const gfx::Model&     t_model,
    renderer::MeshBuffer& t_mesh_buffer
) noexcept -> tl::optional<RenderObject>
{
    std::vector<Node> nodes;
    nodes.reserve(t_model.nodes().size());
    for (const auto& src_node : t_model.nodes()) {
        auto node = create_node(t_device, src_node, nullptr);
        if (!node) {
            return tl::nullopt;
        }
        nodes.push_back(std::move(*node));
    }

    return RenderObject{ std::move(nodes), t_mesh_buffer };
}

auto RenderObject::draw(vk::CommandBuffer t_graphics_buffer) const noexcept
    -> void
{
    m_mesh_buffer.bind(t_graphics_buffer);
    // ...
}

RenderObject::RenderObject(
    std::vector<Node>&&   t_nodes,
    renderer::MeshBuffer& t_mesh
) noexcept
    : m_nodes{ std::move(t_nodes) },
      m_mesh_buffer{ t_mesh }
{}

}   // namespace engine::renderer
