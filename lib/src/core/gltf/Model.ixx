module;

#include <filesystem>
#include <optional>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <fastgltf/core.hpp>

export module core.gltf.Model;

import utility.containers.OptionalRef;

import core.gltf.Image;
import core.gltf.Material;
import core.gltf.Mesh;
import core.gltf.Texture;

namespace core::gltf {

export class Loader;
export class Node;

export class Model {
public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 normal{};
        glm::vec4 tangent{};
        glm::vec2 uv_0{};
        glm::vec2 uv_1{};
        glm::vec4 color{ 1 };
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
};

export class Node {
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
    auto mesh(const Model& model) const
        -> util::OptionalRef<const Mesh>;

private:
    friend Loader;

    std::optional<size_t> m_parent_index;
    std::vector<size_t>   m_child_indices;
    glm::vec3             m_translation{};
    glm::quat             m_rotation{};
    glm::vec3             m_scale{};
    std::optional<size_t> m_mesh_index;
};

export class Loader {
public:
    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& filepath)
        -> std::optional<Model>;

    [[nodiscard]]
    static auto load_from_file(const std::filesystem::path& filepath, size_t scene_index)
        -> std::optional<Model>;

private:
    [[nodiscard]]
    static auto load_model(
        const std::filesystem::path& filepath,
        const fastgltf::Asset&       asset,
        size_t                       scene_index
    ) -> Model;

    static auto load_nodes(
        Model&                                              model,
        const fastgltf::Asset&                              asset,
        const fastgltf::pmr::MaybeSmallVector<std::size_t>& node_indices
    ) -> void;

    static auto load_images(
        Model&                       model,
        const fastgltf::Asset&       asset,
        const std::filesystem::path& filepath
    ) -> void;

    static auto load_materials(Model& model, const fastgltf::Asset& asset) -> void;

    static auto load_node(
        Model&                              model,
        std::vector<Node>&&                 nodes,
        const fastgltf::Asset&              asset,
        const fastgltf::Node&               source_node,
        std::optional<size_t>               parent_index,
        std::unordered_map<size_t, size_t>& node_index_map
    ) -> std::vector<Node>;

    [[nodiscard]]
    static auto load_mesh(
        Model&                 model,
        const fastgltf::Asset& asset,
        const fastgltf::Mesh&  source_mesh
    ) -> size_t;

    [[nodiscard]]
    static auto load_primitive(
        Model&                     model,
        const fastgltf::Asset&     asset,
        const fastgltf::Primitive& source_primitive
    ) -> std::optional<Mesh::Primitive>;

    [[nodiscard]]
    static auto load_vertices(
        Model&                                                    model,
        Mesh::Primitive&                                          primitive,
        const fastgltf::Asset&                                    asset,
        const fastgltf::pmr::SmallVector<fastgltf::Attribute, 4>& attributes
    ) -> bool;

    static auto load_indices(
        Model&                    model,
        Mesh::Primitive&          primitive,
        uint32_t                  first_vertex_index,
        const fastgltf::Asset&    asset,
        const fastgltf::Accessor& accessor
    ) -> void;

    static auto adjust_node_indices(
        Model&                                    model,
        const std::unordered_map<size_t, size_t>& node_index_map
    ) -> void;
};

}   // namespace core::gltf

#include "Node.inl"
