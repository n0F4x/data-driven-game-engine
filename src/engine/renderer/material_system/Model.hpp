#pragma once

#include <vector>

template <typename Vertex>
struct Mesh {
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
};

#include "Mesh.inl"
