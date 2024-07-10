#pragma once

#include <chrono>

#include <glm/glm.hpp>

#include <core/graphics/Camera.hpp>
#include <core/window/Window.hpp>

class Controller {
public:
    explicit Controller(float movement_speed) noexcept;

    auto update(
        const core::window::Window&   window,
        std::chrono::duration<double> delta_time
    ) -> void;

    [[nodiscard]]
    auto update_camera(core::graphics::Camera camera
    ) const noexcept -> core::graphics::Camera;

private:
    struct MovementKeyMap {
        core::window::Key left     = core::window::eA;
        core::window::Key right    = core::window::eD;
        core::window::Key forward  = core::window::eW;
        core::window::Key backward = core::window::eS;
        core::window::Key up       = core::window::eSpace;
        core::window::Key down     = core::window::eLeftShift;
    } m_movement_key_map;

    double m_movement_speed{ 5.f };
    double m_mouse_speed{ 0.002f };

    glm::dvec3 m_position{};
    double     m_horizontal_angle{};
    double     m_vertical_angle{};
};
