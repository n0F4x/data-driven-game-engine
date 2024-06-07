#pragma once

#include <optional>

#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>

namespace core::gltf {

struct Node {
    Node*                 parent;
    glm::vec3             translation;
    glm::quat             rotation;
    glm::vec3             scale;
    std::optional<size_t> mesh_index;
    std::vector<size_t>   child_indices;

    [[nodiscard]]
    auto local_matrix() const -> glm::mat4;
    [[nodiscard]]
    auto matrix() const -> glm::mat4;
};

}   // namespace core::gltf
