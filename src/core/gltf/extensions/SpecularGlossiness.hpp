#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <fastgltf/types.hpp>

#include "core/gltf/Texture.hpp"

namespace core::gltf {

struct SpecularGlossiness {
    struct Material {
        size_t                     material_index;
        glm::vec4                  diffuse_factor{ 1.f };
        std::optional<TextureInfo> diffuse_texture;
        glm::vec3                  specular_factor{ 1.f };
        float                      glossiness_factor{ 1.f };
        std::optional<TextureInfo> specular_glossiness_texture;
    };

    [[nodiscard]]
    static auto
        create_material(const fastgltf::Material& material, size_t base_material_index)
            -> std::optional<Material>;

    std::vector<Material> materials;
};

}   // namespace core::gltf
