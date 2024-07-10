#include "Controller.hpp"

#include <limits>

#include <glm/gtc/matrix_transform.hpp>

Controller::Controller(const float movement_speed) noexcept
    : m_movement_speed{ movement_speed }
{}

auto Controller::update(
    const core::window::Window&   window,
    std::chrono::duration<double> delta_time
) -> void
{
    const glm::dvec2   cursor_position{ window.cursor_position() };
    const core::Size2i window_size{ window.size() };

    m_vertical_angle = glm::mod(
        m_vertical_angle
            + m_mouse_speed
                  * (static_cast<double>(window_size.height) / 2.0 - cursor_position.y),
        glm::two_pi<double>()
    );
    m_horizontal_angle = glm::mod(
        m_horizontal_angle
            + m_mouse_speed
                  * (static_cast<double>(window_size.width) / 2.0 - cursor_position.x),
        glm::two_pi<double>()
    );

    const glm::dvec3     forward{ -glm::sin(m_horizontal_angle),
                              0.f,
                              -glm::cos(m_horizontal_angle) };
    const glm::dvec3     right{ -forward.z, 0, forward.x };
    constexpr glm::dvec3 up{ 0, 1, 0 };

    glm::dvec3 move_dir{};

    if (window.key_pressed(m_movement_key_map.forward)) {
        move_dir += forward;
    }
    if (window.key_pressed(m_movement_key_map.backward)) {
        move_dir -= forward;
    }
    if (window.key_pressed(m_movement_key_map.right)) {
        move_dir += right;
    }
    if (window.key_pressed(m_movement_key_map.left)) {
        move_dir -= right;
    }
    if (window.key_pressed(m_movement_key_map.up)) {
        move_dir += up;
    }
    if (window.key_pressed(m_movement_key_map.down)) {
        move_dir -= up;
    }

    if (glm::dot(move_dir, move_dir) > std::numeric_limits<double>::epsilon()) {
        m_position += m_movement_speed * delta_time.count() * glm::normalize(move_dir);
    }
}

auto Controller::update_camera(core::graphics::Camera camera
) const noexcept -> core::graphics::Camera
{
    camera.set_view_yxz(m_position, glm::vec3{ m_vertical_angle, m_horizontal_angle, 0 });
    return camera;
}
