#pragma once

#include <optional>
#include <ranges>
#include <vector>

#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>

namespace core::gltf {

class Model;

struct Mesh;

class Loader;

class Node {
public:
    Node() = default;
    explicit Node(
        std::optional<size_t>     parent_index  = std::nullopt,
        std::ranges::range auto&& child_indices = std::span<size_t>{},
        std::optional<size_t>     mesh_index    = std::nullopt
    );

    [[nodiscard]]
    auto translation() const noexcept -> const glm::vec3&;
    [[nodiscard]]
    auto translation() noexcept -> glm::vec3&;

    [[nodiscard]]
    auto rotation() const noexcept -> const glm::quat&;
    [[nodiscard]]
    auto rotation() noexcept -> glm::quat&;

    [[nodiscard]]
    auto scale() const noexcept -> const glm::vec3&;
    [[nodiscard]]
    auto scale() noexcept -> glm::vec3&;

    [[nodiscard]]
    auto local_matrix() const -> glm::mat4;
    [[nodiscard]]
    auto matrix(const Model& model) const -> glm::mat4;

    [[nodiscard]]
    auto mesh_index() const noexcept -> std::optional<size_t>;
    [[nodiscard]]
    auto mesh(const Model& model
    ) const -> std::optional<std::reference_wrapper<const Mesh>>;

private:
    friend Loader;

    std::optional<size_t> m_parent_index;
    std::vector<size_t>   m_child_indices;
    glm::vec3             m_translation{};
    glm::quat             m_rotation{};
    glm::vec3             m_scale{};
    std::optional<size_t> m_mesh_index;
};

}   // namespace core::gltf

#include "Node.inl"
