#include "SpecularGlossiness.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace core::gltf {

auto SpecularGlossiness::create_material(
    const fastgltf::Material& t_material,
    size_t                    t_base_material_index
) -> std::optional<Material>
{
    if (t_material.specularGlossiness == nullptr) {
        return std::nullopt;
    }

    const auto& source{ *t_material.specularGlossiness };

    return Material{
        .material_index    = t_base_material_index,
        .diffuse_factor    = glm::make_vec4(source.diffuseFactor.data()),
        .diffuse_texture   = TextureInfo::create(source.diffuseTexture),
        .specular_factor   = glm::make_vec3(source.specularFactor.data()),
        .glossiness_factor = source.glossinessFactor,
        .specular_glossiness_texture =
            TextureInfo::create(source.specularGlossinessTexture),
    };
}

}   // namespace core::gltf
