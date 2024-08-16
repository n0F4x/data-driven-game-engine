#include "Material.hpp"

#include <glm/gtc/type_ptr.hpp>

[[nodiscard]]
static auto convert(const fastgltf::PBRData& source
) -> core::gltf::Material::PbrMetallicRoughness
{
    return core::gltf::Material::PbrMetallicRoughness{
        .base_color_factor       = glm::make_vec4(source.baseColorFactor.data()),
        .base_color_texture_info = core::gltf::TextureInfo::create(source.baseColorTexture
        ),
        .metallic_factor         = source.metallicFactor,
        .roughness_factor        = source.roughnessFactor,
        .metallic_roughness_texture_info =
            core::gltf::TextureInfo::create(source.metallicRoughnessTexture),
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::NormalTextureInfo>& optional
) -> std::optional<core::gltf::Material::NormalTextureInfo>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return core::gltf::Material::NormalTextureInfo{
        .texture_index   = static_cast<uint32_t>(optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(optional->texCoordIndex),
        .scale           = optional->scale,
    };
}

[[nodiscard]]
static auto convert(const fastgltf::Optional<fastgltf::OcclusionTextureInfo>& optional
) -> std::optional<core::gltf::Material::OcclusionTextureInfo>
{
    if (!optional.has_value()) {
        return std::nullopt;
    }
    return core::gltf::Material::OcclusionTextureInfo{
        .texture_index   = static_cast<uint32_t>(optional->textureIndex),
        .tex_coord_index = static_cast<uint32_t>(optional->texCoordIndex),
        .strength        = optional->strength,
    };
}

[[nodiscard]]
static auto convert(fastgltf::AlphaMode alpha_mode
) noexcept -> core::gltf::Material::AlphaMode
{
    using enum core::gltf::Material::AlphaMode;
    switch (alpha_mode) {
        case fastgltf::AlphaMode::Opaque: return eOpaque;
        case fastgltf::AlphaMode::Mask: return eMask;
        case fastgltf::AlphaMode::Blend: return eBlend;
    }
    std::unreachable();
}

namespace core::gltf {

auto Material::create(const fastgltf::Material& material) -> Material
{
    return core::gltf::Material{
        .pbr_metallic_roughness = convert(material.pbrData),
        .normal_texture_info    = convert(material.normalTexture),
        .occlusion_texture_info = convert(material.occlusionTexture),
        .emissive_texture_info  = TextureInfo::create(material.emissiveTexture),
        .emissive_factor        = glm::make_vec3(material.emissiveFactor.data()),
        .alpha_mode             = convert(material.alphaMode),
        .alpha_cutoff           = material.alphaCutoff,
        .double_sided           = material.doubleSided,
    };
}

}   // namespace core::gltf
