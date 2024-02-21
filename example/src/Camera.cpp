#include "Camera.hpp"

#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>

auto Camera::set_perspective_projection(
    float t_fov_y,
    float t_aspect,
    float t_near,
    float t_far
) noexcept -> void
{
    projection = glm::perspective(glm::radians(t_fov_y), t_aspect, t_near, t_far);
    projection[1][1] *= -1.0f;
}

auto Camera::set_view_yxz(glm::vec3 t_position, glm::vec3 t_rotation) noexcept -> void
{
    const float     c3{ glm::cos(t_rotation.z) };
    const float     s3{ glm::sin(t_rotation.z) };
    const float     c2{ glm::cos(t_rotation.x) };
    const float     s2{ glm::sin(t_rotation.x) };
    const float     c1{ glm::cos(t_rotation.y) };
    const float     s1{ glm::sin(t_rotation.y) };
    const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
    const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
    const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };

    view       = glm::mat4{ 1.f };
    view[0][0] = u.x;
    view[1][0] = u.y;
    view[2][0] = u.z;
    view[0][1] = v.x;
    view[1][1] = v.y;
    view[2][1] = v.z;
    view[0][2] = w.x;
    view[1][2] = w.y;
    view[2][2] = w.z;
    view[3][0] = -glm::dot(u, t_position);
    view[3][1] = -glm::dot(v, t_position);
    view[3][2] = -glm::dot(w, t_position);
}
