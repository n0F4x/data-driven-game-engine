module;

#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

module ddge.modules.gfx.Camera;

namespace ddge::gfx {

auto Camera::set_perspective_projection(
    const double fov_y,
    const double aspect,
    const double near,
    const double far
) noexcept -> void
{
    m_projection = glm::perspective(glm::radians(fov_y), aspect, near, far);
    m_projection[1][1] *= -1.f;
}

auto Camera::set_view_yxz(const glm::vec3& position, const glm::vec3& rotation) noexcept
    -> void
{
    const float     c3{ glm::cos(rotation.z) };
    const float     s3{ glm::sin(rotation.z) };
    const float     c2{ glm::cos(rotation.x) };
    const float     s2{ glm::sin(rotation.x) };
    const float     c1{ glm::cos(rotation.y) };
    const float     s1{ glm::sin(rotation.y) };
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
    m_view[3][0] = -glm::dot(u, position);
    m_view[3][1] = -glm::dot(v, position);
    m_view[3][2] = -glm::dot(w, position);
}

auto Camera::position() const noexcept -> glm::vec3
{
    glm::mat4 inverse_view = glm::inverse(m_view);
    return glm::vec3(inverse_view[3]);
}

auto Camera::view() const noexcept -> const glm::mat4&
{
    return m_view;
}

auto Camera::projection() const noexcept -> const glm::mat4&
{
    return m_projection;
}

}   // namespace ddge::gfx
