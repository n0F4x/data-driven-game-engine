module;

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

export module demos.virtual_texture.Camera;

namespace demo {

export struct Camera {
    glm::vec4 position;
    glm::mat4 view;
    glm::mat4 projection;
};

}   // namespace demo
