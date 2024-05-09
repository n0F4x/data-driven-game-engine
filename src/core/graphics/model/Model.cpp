#include "Model.hpp"

#include <glm/gtc/quaternion.hpp>

#include "core/utility/hashing.hpp"

namespace core::graphics {

auto Model::Node::local_matrix() const -> glm::mat4
{
    return glm::translate(glm::mat4(1.f), translation) * glm::mat4_cast(rotation)
         * glm::scale(glm::mat4(1.f), scale);
}

auto Model::Node::matrix() const -> glm::mat4
{
    glm::mat4 result{ local_matrix() };
    for (const Node* p{ parent }; p != nullptr; p = p->parent) {
        result = p->local_matrix() * result;
    }
    return result;
}

auto Model::hash(
    const std::filesystem::path& t_filepath,
    const std::optional<size_t>   t_scene_id
) noexcept -> size_t
{
    return hash_combine(t_filepath, t_scene_id);
}

auto Model::vertices() const noexcept -> const std::vector<Vertex>&
{
    return m_vertices;
}

auto Model::indices() const noexcept -> const std::vector<uint32_t>&
{
    return m_indices;
}

auto Model::images() const noexcept -> const std::vector<Image>&
{
    return m_images;
}

auto Model::samplers() const noexcept -> const std::vector<Sampler>&
{
    return m_samplers;
}

auto Model::textures() const noexcept -> const std::vector<Texture>&
{
    return m_textures;
}

auto Model::materials() const noexcept -> const std::vector<Material>&
{
    return m_materials;
}

auto Model::meshes() const noexcept -> const std::vector<Mesh>&
{
    return m_meshes;
}

auto Model::nodes() const noexcept -> const std::vector<Node>&
{
    return m_nodes;
}

auto Model::root_node_indices() const noexcept -> const std::vector<size_t>&
{
    return m_root_node_indices;
}

}   // namespace core::graphics
