#pragma once

#include <chrono>

#include <glm/glm.hpp>

struct Camera {
    auto set_perspective_projection(
        float t_fov_y,
        float t_aspect,
        float t_near,
        float t_far
    ) noexcept -> void;

    auto set_view_yxz(glm::vec3 t_position, glm::vec3 t_rotation) noexcept -> void;

    glm::mat4 projection{ 1.f };
    glm::mat4 view{ 1.f };
};
