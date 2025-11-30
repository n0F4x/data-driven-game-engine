module;

#include <optional>
#include <utility>

#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/types.hpp>

module ddge.deprecated.gltf.Material;

import ddge.deprecated.gltf.Texture;

[[nodiscard]]
static auto convert(const fastgltf::PBRData& source)
    -> ddge::gltf::Material::PbrMetallicRoughness
{
    return ddge::gltf::Material::PbrMetallicRoughness{
        .base_color_factor = glm::make_vec4(source.baseColorFactor.data()),
        .base_color_texture_info =
            source.baseColorTexture.transform(ddge::gltf::TextureInfo::create),
        .metallic_factor  = source.metallicFactor,
        .roughness_factor = source.roughnessFactor,
        .metallic_roughness_texture_info =
            source.metallicRoughnessTexture.transform(ddge::gltf::TextureInfo::create),
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::NormalTextureInfo>& optional)
    -> std::optional<ddge::gltf::Material::NormalTextureInfo>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return ddge::gltf::Material::NormalTextureInfo{
        .texture_index   = static_cast<uint32_t>(optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(optional->texCoordIndex),
        .scale           = optional->scale,
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::OcclusionTextureInfo>& optional)
    -> std::optional<ddge::gltf::Material::OcclusionTextureInfo>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return ddge::gltf::Material::OcclusionTextureInfo{
        .texture_index   = static_cast<uint32_t>(optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(optional->texCoordIndex),
        .strength        = optional->strength,
    };
}

[[nodiscard]]
static auto convert(const fastgltf::AlphaMode alpha_mode) noexcept
    -> ddge::gltf::Material::AlphaMode
{
    using enum ddge::gltf::Material::AlphaMode;
    switch (alpha_mode) {
        case fastgltf::AlphaMode::Opaque: return eOpaque;
        case fastgltf::AlphaMode::Mask:   return eMask;
        case fastgltf::AlphaMode::Blend:  return eBlend;
    }
    std::unreachable();
}

namespace ddge::gltf {

auto Material::create(const fastgltf::Material& material) -> Material
{
    return Material{
        .pbr_metallic_roughness = ::convert(material.pbrData),
        .normal_texture_info    = ::convert(material.normalTexture),
        .occlusion_texture_info = ::convert(material.occlusionTexture),
        .emissive_texture_info  = material.emissiveTexture.transform(TextureInfo::create),
        .emissive_factor        = glm::make_vec3(material.emissiveFactor.data()),
        .alpha_mode             = ::convert(material.alphaMode),
        .alpha_cutoff           = material.alphaCutoff,
        .double_sided           = material.doubleSided,
    };
}

}   // namespace ddge::gltf
