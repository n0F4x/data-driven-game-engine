module;

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

export module demos.virtual_texture.Vertex;

namespace demo {

export struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
};

}   // namespace demo
