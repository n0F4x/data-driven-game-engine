#include "SpecularGlossiness.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace core::gltf {

auto SpecularGlossiness::create_material(
    const fastgltf::Material& material,
    const size_t              base_material_index
) -> std::optional<Material>
{
    if (material.specularGlossiness == nullptr) {
        return std::nullopt;
    }

    const auto& [diffuse_factor, diffuse_texture, specular_factor, glossiness_factor, texture]{
        *material.specularGlossiness
    };

    return Material{
        .material_index              = base_material_index,
        .diffuse_factor              = glm::make_vec4(diffuse_factor.data()),
        .diffuse_texture             = diffuse_texture.transform(TextureInfo::create),
        .specular_factor             = glm::make_vec3(specular_factor.data()),
        .glossiness_factor           = glossiness_factor,
        .specular_glossiness_texture = texture.transform(TextureInfo::create),
    };
}

}   // namespace core::gltf
