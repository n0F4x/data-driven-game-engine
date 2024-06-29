#include "Controller.hpp"

#include <limits>

#include <glm/gtc/matrix_transform.hpp>

Controller::Controller(const float movement_speed) noexcept
    : m_movement_speed{ movement_speed }
{}

auto Controller::update(const core::window::Window& t_window, float t_delta_time) noexcept
    -> void
{
    double cursor_x{};
    double cursor_y{};
    glfwGetCursorPos(t_window.get(), &cursor_x, &cursor_y);

    int width{};
    int height{};
    glfwGetWindowSize(t_window.get(), &width, &height);

    m_vertical_angle = glm::mod(
        m_vertical_angle
            + m_mouse_speed
                  * (static_cast<float>(height) / 2.f - static_cast<float>(cursor_y)),
        glm::two_pi<float>()
    );
    m_horizontal_angle = glm::mod(
        m_horizontal_angle
            + m_mouse_speed
                  * (static_cast<float>(width) / 2.f - static_cast<float>(cursor_x)),
        glm::two_pi<float>()
    );

    const glm::vec3     forward{ -glm::sin(m_horizontal_angle),
                             0.f,
                             -glm::cos(m_horizontal_angle) };
    const glm::vec3     right{ -forward.z, 0.f, forward.x };
    constexpr glm::vec3 up{ 0.f, 1.f, 0.f };

    glm::vec3 move_dir{};
    if (glfwGetKey(t_window.get(), m_movement_key_map.forward) == GLFW_PRESS) {
        move_dir += forward;
    }
    if (glfwGetKey(t_window.get(), m_movement_key_map.backward) == GLFW_PRESS) {
        move_dir -= forward;
    }
    if (glfwGetKey(t_window.get(), m_movement_key_map.right) == GLFW_PRESS) {
        move_dir += right;
    }
    if (glfwGetKey(t_window.get(), m_movement_key_map.left) == GLFW_PRESS) {
        move_dir -= right;
    }
    if (glfwGetKey(t_window.get(), m_movement_key_map.up) == GLFW_PRESS) {
        move_dir += up;
    }
    if (glfwGetKey(t_window.get(), m_movement_key_map.down) == GLFW_PRESS) {
        move_dir -= up;
    }

    if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon()) {
        m_position += m_movement_speed * t_delta_time * glm::normalize(move_dir);
    }
}

auto Controller::update_camera(core::graphics::Camera t_camera
) const noexcept -> core::graphics::Camera
{
    t_camera.set_view_yxz(
        m_position, glm::vec3{ m_vertical_angle, m_horizontal_angle, 0 }
    );
    return t_camera;
}
