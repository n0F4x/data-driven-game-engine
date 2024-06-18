#include "Material.hpp"

#include <glm/gtc/type_ptr.hpp>

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::NormalTextureInfo>& t_optional
) -> std::optional<core::gltf::Material::NormalTextureInfo>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return core::gltf::Material::NormalTextureInfo{
        .texture_index   = static_cast<uint32_t>(t_optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(t_optional->texCoordIndex),
        .scale           = t_optional->scale,
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::OcclusionTextureInfo>& t_optional
) -> std::optional<core::gltf::Material::OcclusionTextureInfo>
{
    if (!t_optional.has_value()) {
        return std::nullopt;
    }
    return core::gltf::Material::OcclusionTextureInfo{
        .texture_index   = static_cast<uint32_t>(t_optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(t_optional->texCoordIndex),
        .strength        = t_optional->strength,
    };
}

[[nodiscard]]
static auto convert(fastgltf::AlphaMode t_alpha_mode
) noexcept -> core::gltf::Material::AlphaMode
{
    using enum core::gltf::Material::AlphaMode;
    switch (t_alpha_mode) {
        case fastgltf::AlphaMode::Opaque: return eOpaque;
        case fastgltf::AlphaMode::Mask: return eMask;
        case fastgltf::AlphaMode::Blend: return eBlend;
    }
    std::unreachable();
}

namespace core::gltf {

auto Material::create(const fastgltf::Material& t_material) -> Material
{
    using PbrMetallicRoughness = Material::PbrMetallicRoughness;
    return core::gltf::Material{
        .pbr_metallic_roughness =
            PbrMetallicRoughness{
                                 .base_color_factor =
                    glm::make_vec4(t_material.pbrData.baseColorFactor.data()),
                                 .base_color_texture_info =
                    TextureInfo::create(t_material.pbrData.baseColorTexture),
                                 .metallic_factor  = t_material.pbrData.metallicFactor,
                                 .roughness_factor = t_material.pbrData.roughnessFactor,
                                 .metallic_roughness_texture_info =
                    TextureInfo::create(t_material.pbrData.metallicRoughnessTexture),
                                 },
        .normal_texture_info    = convert(t_material.normalTexture),
        .occlusion_texture_info = convert(t_material.occlusionTexture),
        .emissive_texture_info  = TextureInfo::create(t_material.emissiveTexture),
        .emissive_factor        = glm::make_vec3(t_material.emissiveFactor.data()),
        .alpha_mode             = convert(t_material.alphaMode),
        .alpha_cutoff           = t_material.alphaCutoff,
        .double_sided           = t_material.doubleSided
    };
}

}   // namespace core::gltf
