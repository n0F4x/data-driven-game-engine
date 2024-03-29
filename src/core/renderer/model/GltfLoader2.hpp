#pragma once

#include <unordered_map>

#include "Model.hpp"

namespace core::renderer::detail {

struct GltfLoader2 {
    std::vector<size_t>                root_nodes;
    std::unordered_map<size_t, size_t> node_indices;
    std::vector<Model::Node>           nodes;

    std::vector<Model::Vertex> vertices;
    std::vector<uint32_t>      indices;
};

}   // namespace core::renderer::detail
