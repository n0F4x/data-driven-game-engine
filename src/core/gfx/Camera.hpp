#pragma once

#include <glm/glm.hpp>

namespace core::gfx {

class Camera {
public:
    auto set_perspective_projection(double fov_y, double aspect, double near, double far)
        noexcept -> void;

    auto set_view_yxz(const glm::vec3& position, const glm::vec3& rotation) noexcept
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

}   // namespace core::gfx
