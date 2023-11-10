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

}   // namespace engine::gfx
