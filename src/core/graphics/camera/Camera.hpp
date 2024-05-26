#pragma once

#include <glm/glm.hpp>

namespace core::graphics {

class Camera {
public:
    auto set_perspective_projection(
        float t_fov_y,
        float t_aspect,
        float t_near,
        float t_far
    ) noexcept -> void;

    auto set_view_yxz(const glm::vec3& t_position, const glm::vec3& t_rotation) noexcept
        -> void;


    [[nodiscard]]
    auto position() const noexcept -> glm::vec3;
    [[nodiscard]]
    auto view() const noexcept -> const glm::mat4&;
    [[nodiscard]]
    auto projection() const noexcept -> const glm::mat4&;

private:
    glm::mat4 m_view{ 1.f };
    glm::mat4 m_projection{ 1.f };
};

}   // namespace core::graphics
