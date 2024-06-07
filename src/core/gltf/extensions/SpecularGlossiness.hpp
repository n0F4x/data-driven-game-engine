#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace core::gltf {

struct SpecularGlossiness {
    struct Material {
        size_t    material_index;
        glm::vec4 diffuse;
    };

    std::vector<Material> materials;
};

}   // namespace core::gltf
