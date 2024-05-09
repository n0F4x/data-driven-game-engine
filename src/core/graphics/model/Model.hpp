#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <tl/optional.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <fastgltf/core.hpp>

#include "core/asset/image/Image.hpp"

namespace core::graphics {

class GltfLoader;

class Model {
public:
    struct Vertex {
        glm::vec4 position{};
        glm::vec4 normal{};
        glm::vec4 tangent{};
        glm::vec2 uv_0{};
        glm::vec2 uv_1{};
        glm::vec4 color{};
    };

    using Image = std::unique_ptr<asset::Image>;

    struct Sampler {
        enum class MagFilter {
            eNearest,
            eLinear
        };
        enum class MinFilter {
            eNearest,
            eLinear,
            eNearestMipmapNearest,
            eLinearMipmapNearest,
            eNearestMipmapLinear,
            eLinearMipmapLinear,
        };

        enum class WrapMode {
            eClampToEdge,
            eMirroredRepeat,
            eRepeat
        };

        std::optional<MagFilter> magFilter;
        std::optional<MinFilter> minFilter;
        WrapMode                 wrapS{ WrapMode::eRepeat };
        WrapMode                 wrapT{ WrapMode::eRepeat };
    };

    struct Texture {
        std::optional<size_t> sampler_index;
        size_t               image_index;
    };

    struct TextureInfo {
        size_t texture_index;
        size_t tex_coord_index{};
    };

    struct Material {
        struct PbrMetallicRoughness {
            glm::vec4                 baseColorFactor{ 1.f };
            TextureInfo               baseColorTexture;
            float                     metallicFactor{ 1.f };
            float                     roughnessFactor{ 1.f };
            tl::optional<TextureInfo> metallicRoughnessTexture;
        };

        struct NormalTextureInfo {
            size_t texture_index;
            size_t tex_coord_index{};
            float  scale{ 1.f };
        };

        struct OcclusionTextureInfo {
            size_t texture_index;
            size_t tex_coord_index{};
            float  strength{ 1.f };
        };

        enum class AlphaMode {
            eOpaque,
            eMask,
            eBlend
        };

        tl::optional<PbrMetallicRoughness> pbrMetallicRoughness;
        tl::optional<NormalTextureInfo>    normalTextureInfo;
        tl::optional<OcclusionTextureInfo> occlusionTextureInfo;
        tl::optional<TextureInfo>          emissiveTexture;
        glm::vec3                          emissiveFactor{};
        AlphaMode                          alphaMode{ AlphaMode::eOpaque };
        float                              alphaCutoff{ 0.5f };
        bool                               doubleSided{ false };
    };

    struct Primitive {
        enum class Topology {
            ePoints,
            eLineStrips,
            eLineLoops,
            eLines,
            eTriangles,
            eTriangleStrips,
            eTriangleFans
        };

        Topology             mode;
        tl::optional<size_t> material_index;
        uint32_t             first_index_index;
        uint32_t             index_count;
        uint32_t             vertex_count;
    };

    struct Mesh {
        std::vector<Primitive> primitives;
    };

    struct Node {
        Node*                parent;
        glm::vec3            translation;
        glm::quat            rotation;
        glm::vec3            scale;
        tl::optional<size_t> mesh_index;
        std::vector<size_t>  child_indices;

        [[nodiscard]]
        auto local_matrix() const -> glm::mat4;
        [[nodiscard]]
        auto matrix() const -> glm::mat4;
    };

    [[nodiscard]]
    static auto hash(
        const std::filesystem::path& t_filepath,
        tl::optional<size_t>         t_scene_id
    ) noexcept -> size_t;

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
    friend GltfLoader;

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

}   // namespace core::graphics
