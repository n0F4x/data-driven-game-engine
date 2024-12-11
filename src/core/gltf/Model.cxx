module;

#include <filesystem>
#include <optional>

module core.gltf.Model;

import utility.hashing;

import core.gltf.Texture;

namespace core::gltf {

auto Model::hash(
    const std::filesystem::path& filepath,
    const std::optional<size_t>  scene_index
) noexcept -> size_t
{
    return hash_combine(filepath, scene_index);
}

auto Model::default_sampler() -> const Sampler&
{
    constexpr static Sampler s_default_sampler{};
    return s_default_sampler;
}

auto Model::default_material() -> const Material&
{
    constexpr static Material s_default_material{};
    return s_default_material;
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

}   // namespace core::gltf
