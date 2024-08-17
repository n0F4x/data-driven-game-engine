#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <fastgltf/core.hpp>

#include "core/gltf/extensions/SpecularGlossiness.hpp"

#include "Image.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Node.hpp"
#include "Texture.hpp"

namespace core::gltf {

class Loader;

class Model {
public:
    struct Vertex {
        glm::vec4 position{};
        glm::vec4 normal{};
        glm::vec4 tangent{};
        glm::vec2 uv_0{};
        glm::vec2 uv_1{};
        glm::vec4 color{ 1 };
    };

    struct Extensions {
        SpecularGlossiness specular_glossiness;
    };

    [[nodiscard]]
    static auto hash(
        const std::filesystem::path& filepath,
        std::optional<size_t>        scene_index
    ) noexcept -> size_t;

    [[nodiscard]]
    static auto default_sampler() -> const Sampler&;

    [[nodiscard]]
    static auto default_material() -> const Material&;

    [[nodiscard]]
    auto vertices() const noexcept -> const std::vector<Vertex>&;
    [[nodiscard]]
    auto indices() const noexcept -> const std::vector<uint32_t>&;
    [[nodiscard]]
    auto images() const noexcept -> const std::vector<Image>&;
    [[nodiscard]]
    auto samplers() const noexcept -> const std::vector<Sampler>&;
    [[nodiscard]]
    auto textures() const noexcept -> const std::vector<Texture>&;
    [[nodiscard]]
    auto materials() const noexcept -> const std::vector<Material>&;
    [[nodiscard]]
    auto meshes() const noexcept -> const std::vector<Mesh>&;
    [[nodiscard]]
    auto nodes() const noexcept -> const std::vector<Node>&;
    [[nodiscard]]
    auto root_node_indices() const noexcept -> const std::vector<size_t>&;
    [[nodiscard]]
    auto extensions() const noexcept -> const Extensions&;

private:
    friend Loader;

    std::vector<Vertex>   m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Image>    m_images;
    std::vector<Sampler>  m_samplers;
    std::vector<Texture>  m_textures;
    std::vector<Material> m_materials;
    std::vector<Mesh>     m_meshes;
    std::vector<Node>     m_nodes;
    std::vector<size_t>   m_root_node_indices;
    Extensions            m_extensions;
};

}   // namespace core::gltf
