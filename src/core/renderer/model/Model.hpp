#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <tl/optional.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <fastgltf/core.hpp>

#include "Image.hpp"
#include "Vertex.hpp"

namespace core::renderer {

class ModelLoader;

class Model {
public:
    using Image = Image;

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

        enum class WrappingMode {
            eClampToEdge,
            eMirroredRepeat,
            eRepeat
        };

        tl::optional<MagFilter> magFilter;
        tl::optional<MinFilter> minFilter;
        WrappingMode            wrapS{ WrappingMode::eRepeat };
        WrappingMode            wrapT{ WrappingMode::eRepeat };
    };

    struct Texture {
        tl::optional<size_t> sampler;
        size_t               source;
    };

    struct TextureInfo {
        size_t index;
        size_t texCoord{};
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
            size_t index;
            size_t texCoord{};
            float  scale{ 1.f };
        };

        struct OcclusionTextureInfo {
            size_t index;
            size_t texCoord{};
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
        tl::optional<size_t> material;
        uint32_t             first_index_index;
        uint32_t             index_count;
        uint32_t             vertex_count;
    };

    struct Mesh {
        std::vector<Primitive> primitives;
    };

    struct Node {
        Node*               parent;
        std::vector<size_t> children;
        glm::vec3           translation;
        glm::quat           rotation;
        glm::vec3           scale;
        tl::optional<Mesh>  mesh;

        [[nodiscard]] auto local_matrix() const -> glm::mat4;
        [[nodiscard]] auto matrix() const -> glm::mat4;
    };

    [[nodiscard]] static auto hash(const std::filesystem::path& t_filepath) noexcept -> size_t;

private:
    friend ModelLoader;

    std::filesystem::path m_filepath;
    std::vector<float>    m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<Image>    images;
    std::vector<Sampler>  samplers;
    std::vector<Texture>  textures;
    std::vector<Material> materials;
    std::vector<Node>     nodes;

    friend auto hash_value(const Model& t_model) noexcept -> size_t;
};

}   // namespace core::renderer

namespace std {

template <>
class hash<core::renderer::Model> {
public:
    [[nodiscard]] auto operator()(const core::renderer::Model& t_model) const -> size_t;
};

}   // namespace std
