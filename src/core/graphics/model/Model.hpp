#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

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

        std::optional<MagFilter> mag_filter;
        std::optional<MinFilter> min_filter;
        WrapMode                 wrap_s{ WrapMode::eRepeat };
        WrapMode                 wrap_t{ WrapMode::eRepeat };
    };

    struct Texture {
        std::optional<uint32_t> sampler_index;
        uint32_t                image_index;
    };

    struct TextureInfo {
        uint32_t texture_index;
        uint32_t tex_coord_index{};
    };

    struct Material {
        struct PbrMetallicRoughness {
            glm::vec4                  base_color_factor{ 1.f };
            std::optional<TextureInfo> base_color_texture_info;
            float                      metallic_factor{ 1.f };
            float                      roughness_factor{ 1.f };
            std::optional<TextureInfo> metallic_roughness_texture_info;
        };

        struct NormalTextureInfo {
            uint32_t texture_index;
            uint32_t tex_coord_index{};
            float    scale{ 1.f };
        };

        struct OcclusionTextureInfo {
            uint32_t texture_index;
            uint32_t tex_coord_index{};
            float    strength{ 1.f };
        };

        enum class AlphaMode {
            eOpaque,
            eMask,
            eBlend
        };

        PbrMetallicRoughness                pbr_metallic_roughness;
        std::optional<NormalTextureInfo>    normal_texture_info;
        std::optional<OcclusionTextureInfo> occlusion_texture_info;
        std::optional<TextureInfo>          emissive_texture_info;
        glm::vec3                           emissive_factor{};
        AlphaMode                           alpha_mode{ AlphaMode::eOpaque };
        float                               alpha_cutoff{ 0.5f };
        bool                                double_sided{ false };
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

        Topology                mode;
        std::optional<uint32_t> material_index;
        uint32_t                first_index_index;
        uint32_t                index_count;
        uint32_t                vertex_count;
    };

    struct Mesh {
        std::vector<Primitive> primitives;
    };

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

    [[nodiscard]]
    static auto hash(
        const std::filesystem::path& t_filepath,
        std::optional<size_t>        t_scene_id
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
