#pragma once

#include <chrono>
#include <numbers>

#include <glm/glm.hpp>

#include <engine/window/Window.hpp>

#include "Camera.hpp"

class Controller {
public:
    auto update(
        const engine::window::Window& t_window,
        float                         t_delta_time
    ) noexcept -> void;
    [[nodiscard]] auto update_camera(Camera t_camera) noexcept -> Camera;

private:
    struct MovementKeyMap {
        int left     = GLFW_KEY_A;
        int right    = GLFW_KEY_D;
        int forward  = GLFW_KEY_W;
        int backward = GLFW_KEY_S;
        int up       = GLFW_KEY_SPACE;
        int down     = GLFW_KEY_LEFT_SHIFT;
    } m_movement_key_map;

    float m_movement_speed{ 5.f };
    float m_mouse_speed{ 0.002f };

    glm::vec3 m_position{};
    float     m_horizontal_angle{};
    float     m_vertical_angle{};
};
