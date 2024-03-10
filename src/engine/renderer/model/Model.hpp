#pragma once

#include <string>
#include <vector>

#include <tl/optional.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Image.hpp"
#include "Vertex.hpp"

namespace engine::renderer {

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

        std::string             name;
        tl::optional<MagFilter> magFilter;
        tl::optional<MinFilter> minFilter;
        WrappingMode            wrapS{ WrappingMode::eRepeat };
        WrappingMode            wrapT{ WrappingMode::eRepeat };
    };

    struct Texture {
        std::string          name;
        tl::optional<size_t> sampler;
        tl::optional<size_t> source;
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

        std::string                        name;
        tl::optional<PbrMetallicRoughness> pbrMetallicRoughness;
        tl::optional<NormalTextureInfo>    normalTextureInfo;
        tl::optional<OcclusionTextureInfo> occlusionTextureInfo;
        tl::optional<TextureInfo>          emissiveTexture;
        glm::vec3                          emissiveFactor{};
        AlphaMode                          alphaMode{ AlphaMode::eOpaque };
        float                              alphaCutoff{ 0.5f };
        bool                               doubleSided{ false };
    };

    enum class Topology {
        ePoints,
        eLineStrips,
        eLineLoops,
        eLines,
        eTriangles,
        eTriangleStrips,
        eTriangleFans
    };

    struct Primitive {
        Topology             mode;
        tl::optional<size_t> material;
        uint32_t             first_index_index;
        uint32_t             index_count;
        uint32_t             vertex_count;
    };

    struct Mesh {
        std::string            name;
        std::vector<Primitive> primitives;
    };

    struct Node {
        std::string         name;
        Node*               parent;
        std::vector<size_t> children;
        glm::vec3           translation;
        glm::quat           rotation;
        glm::vec3           scale;
        tl::optional<Mesh>  mesh;

        [[nodiscard]] auto local_matrix() const -> glm::mat4;
        [[nodiscard]] auto matrix() const -> glm::mat4;
    };

    struct Scene {
        std::string         name;
        std::vector<size_t> nodes;
    };

private:
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    std::vector<Node>     nodes;
    std::vector<Scene>    scenes;
    std::vector<size_t>   scene;
};

}   // namespace engine::renderer
