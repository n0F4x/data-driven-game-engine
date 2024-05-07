#include "Camera.hpp"

#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>

namespace core::graphics {

auto Camera::set_perspective_projection(
    const float t_fov_y,
    const float t_aspect,
    const float t_near,
    const float t_far
) noexcept -> void
{
    m_projection = glm::perspective(glm::radians(t_fov_y), t_aspect, t_near, t_far);
    m_projection[1][1] *= -1.f;
}

auto Camera::set_view_yxz(const glm::vec3& t_position, const glm::vec3& t_rotation) noexcept
    -> void
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

    m_view       = glm::mat4{ 1.f };
    m_view[0][0] = u.x;
    m_view[1][0] = u.y;
    m_view[2][0] = u.z;
    m_view[0][1] = v.x;
    m_view[1][1] = v.y;
    m_view[2][1] = v.z;
    m_view[0][2] = w.x;
    m_view[1][2] = w.y;
    m_view[2][2] = w.z;
    m_view[3][0] = -glm::dot(u, t_position);
    m_view[3][1] = -glm::dot(v, t_position);
    m_view[3][2] = -glm::dot(w, t_position);
}

auto Camera::view() const noexcept -> const glm::mat4&
{
    return m_view;
}

auto Camera::projection() const noexcept -> const glm::mat4&
{
    return m_projection;
}

}   // namespace core::graphics
