#include "RenderScene.hpp"

namespace engine::renderer {

auto RenderScene::load(const Device& t_device, gfx::Model t_model) noexcept
    -> tl::optional<ModelHandle>
{
    auto opt_mesh_buffer{ renderer::MeshBuffer::create<gfx::Model::Vertex>(
        t_device, t_model.vertices(), t_model.indices()
    ) };
    if (!opt_mesh_buffer) {
        return tl::nullopt;
    }
    auto [staging_mesh_buffer, mesh_buffer]{ std::move(*opt_mesh_buffer) };

    m_staging_mesh_buffers.push_back(std::move(staging_mesh_buffer));

    auto unique_mesh_buffer{ std::make_unique<MeshBuffer>(std::move(mesh_buffer)
    ) };
    auto unique_model{ std::make_unique<gfx::Model>(std::move(t_model)) };
    ModelHandle handle{ *unique_mesh_buffer, *unique_model };

    m_mesh_buffers.push_back(std::move(unique_mesh_buffer));
    m_models.push_back(std::move(unique_model));

    return handle;
}

auto RenderScene::flush(vk::CommandBuffer t_copy_buffer) noexcept -> void
{
    for (const auto& staging_mesh_buffer : m_staging_mesh_buffers) {
        staging_mesh_buffer.upload(t_copy_buffer);
    }
    m_staging_mesh_buffers.clear();
}

auto ModelHandle::spawn(const Device& t_device) const noexcept
    -> tl::optional<RenderObject>
{
    return RenderObject::create(t_device, m_model, m_mesh_buffer);
}

ModelHandle::ModelHandle(
    MeshBuffer& t_mesh_buffer,
    gfx::Model& t_model
) noexcept
    : m_mesh_buffer{ t_mesh_buffer },
      m_model{ t_model }
{}

}   // namespace engine::renderer
