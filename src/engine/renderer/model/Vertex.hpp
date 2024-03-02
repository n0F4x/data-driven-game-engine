#pragma once

#include <glm/glm.hpp>

namespace engine::renderer {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 uv_0;
    glm::vec2 uv_1;
    glm::vec4 color;
};

}
