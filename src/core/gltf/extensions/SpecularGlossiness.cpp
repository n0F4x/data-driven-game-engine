#include "SpecularGlossiness.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace core::gltf {

auto SpecularGlossiness::create_material(
    const fastgltf::Material& material,
    size_t                    base_material_index
) -> std::optional<Material>
{
    if (material.specularGlossiness == nullptr) {
        return std::nullopt;
    }

    const fastgltf::MaterialSpecularGlossiness& source{ *material.specularGlossiness };

    return Material{
        .material_index    = base_material_index,
        .diffuse_factor    = glm::make_vec4(source.diffuseFactor.data()),
        .diffuse_texture   = source.diffuseTexture.transform(TextureInfo::create),
        .specular_factor   = glm::make_vec3(source.specularFactor.data()),
        .glossiness_factor = source.glossinessFactor,
        .specular_glossiness_texture =
            source.specularGlossinessTexture.transform(TextureInfo::create),
    };
}

}   // namespace core::gltf
