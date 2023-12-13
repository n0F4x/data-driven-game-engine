#include "Model.hpp"

namespace engine::gfx {

Model::Model(
    std::vector<Vertex>&&   t_vertices,
    std::vector<uint32_t>&& t_indices,
    std::vector<Node>&&     t_nodes
) noexcept
    : m_vertices{ std::move(t_vertices) },
      m_indices{ std::move(t_indices) },
      m_nodes{ std::move(t_nodes) }
{}

auto Model::vertices() const noexcept -> const std::vector<Vertex>&
{
    return m_vertices;
}

auto Model::indices() const noexcept -> const std::vector<uint32_t>&
{
    return m_indices;
}

auto Model::nodes() const noexcept -> const std::vector<Node>&
{
    return m_nodes;
}

}   // namespace engine::gfx
