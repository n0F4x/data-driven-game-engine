module;

#include <optional>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <fastgltf/types.hpp>

export module ddge.modules.gltf.Material;

import ddge.modules.gltf.Texture;

namespace ddge::gltf {

// NOLINTNEXTLINE(*-member-init)
export struct Material {
    struct PbrMetallicRoughness {
        glm::vec4                  base_color_factor{ 1.f };
        std::optional<TextureInfo> base_color_texture_info{};
        float                      metallic_factor{ 1.f };
        float                      roughness_factor{ 1.f };
        std::optional<TextureInfo> metallic_roughness_texture_info{};
    };

    // NOLINTNEXTLINE(*-member-init)
    struct NormalTextureInfo {
        uint32_t texture_index;
        uint32_t tex_coord_index{};
        float    scale{ 1.f };
    };

    // NOLINTNEXTLINE(*-member-init)
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
    glm::vec3                           emissive_factor{ 0.f };
    AlphaMode                           alpha_mode{ AlphaMode::eOpaque };
    float                               alpha_cutoff{ 0.5f };
    bool                                double_sided{ false };

    [[nodiscard]]
    static auto create(const fastgltf::Material& material) -> Material;
};

}   // namespace ddge::gltf
