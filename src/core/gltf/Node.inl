namespace core::gltf {

Node::Node(
    std::optional<size_t>     parent_index,
    std::ranges::range auto&& child_indices,
    std::optional<size_t>     mesh_index
)
    : m_parent_index{ parent_index },
      // TODO: use from_range
      m_child_indices{ child_indices.begin(), child_indices.end() },
      m_mesh_index{ mesh_index }
{}

}   // namespace core::gltf
