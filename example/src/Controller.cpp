#include "Controller.hpp"

#include <limits>

#include <glm/gtc/matrix_transform.hpp>

auto Controller::update(float t_delta_time) noexcept -> void
{
    glm::vec3 rotate{ 0 };
    if (sf::Keyboard::isKeyPressed(m_turning_key_map.right)) {
        rotate.y -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(m_turning_key_map.left)) {
        rotate.y += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(m_turning_key_map.up)) {
        rotate.x += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(m_turning_key_map.down)) {
        rotate.x -= 1.f;
    }

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        m_rotation += m_turning_speed * t_delta_time * glm::normalize(rotate);
    }

    m_rotation.x = glm::mod(m_rotation.x, glm::two_pi<float>());
    m_rotation.y = glm::mod(m_rotation.y, glm::two_pi<float>());

    float           yaw = m_rotation.y;
    const glm::vec3 forward_dir{ -sin(yaw), 0.f, -cos(yaw) };
    const glm::vec3 right_dir{ -forward_dir.z, 0.f, forward_dir.x };
    const glm::vec3 up_dir{ 0.f, 1.f, 0.f };

    glm::vec3 move_dir{ 0.f };
    if (sf::Keyboard::isKeyPressed(m_movement_key_map.forward)) {
        move_dir += forward_dir;
    }
    if (sf::Keyboard::isKeyPressed(m_movement_key_map.backward)) {
        move_dir -= forward_dir;
    }
    if (sf::Keyboard::isKeyPressed(m_movement_key_map.right)) {
        move_dir += right_dir;
    }
    if (sf::Keyboard::isKeyPressed(m_movement_key_map.left)) {
        move_dir -= right_dir;
    }
    if (sf::Keyboard::isKeyPressed(m_movement_key_map.up)) {
        move_dir += up_dir;
    }
    if (sf::Keyboard::isKeyPressed(m_movement_key_map.down)) {
        move_dir -= up_dir;
    }

    if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon()) {
        m_translation +=
            m_movement_speed * t_delta_time * glm::normalize(move_dir);
    }
}

auto Controller::update_camera(Camera t_camera) noexcept -> Camera
{
    t_camera.set_view_yxz(m_translation, m_rotation);
    return t_camera;
}
