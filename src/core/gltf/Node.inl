#pragma once

namespace core::gltf {

Node::Node(
    const std::optional<size_t> parent_index,
    std::ranges::range auto&&   child_indices,
    const std::optional<size_t> mesh_index
)
    : m_parent_index{ parent_index },
      m_child_indices{ std::from_range,
                       std::forward<decltype(child_indices)>(child_indices) },
      m_mesh_index{ mesh_index }
{}

}   // namespace core::gltf
