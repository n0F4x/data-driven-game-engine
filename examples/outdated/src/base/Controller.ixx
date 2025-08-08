module;

#include <chrono>

#include <glm/glm.hpp>

export module examples.base.Controller;

import modules.gfx.Camera;
import modules.window.Key;
import modules.window.Window;

namespace examples::base {

export class Controller {
public:
    explicit Controller(float movement_speed) noexcept;

    auto update(
        const modules::window::Window&   window,
        std::chrono::duration<double> delta_time
    ) -> void;

    [[nodiscard]]
    auto update_camera(modules::gfx::Camera camera) const noexcept -> modules::gfx::Camera;

private:
    struct MovementKeyMap {
        modules::window::Key left     = modules::window::eA;
        modules::window::Key right    = modules::window::eD;
        modules::window::Key forward  = modules::window::eW;
        modules::window::Key backward = modules::window::eS;
        modules::window::Key up       = modules::window::eSpace;
        modules::window::Key down     = modules::window::eLeftShift;
    } m_movement_key_map;

    double m_movement_speed{ 5.f };
    double m_mouse_speed{ 0.002f };

    glm::dvec3 m_position{};
    double     m_horizontal_angle{};
    double     m_vertical_angle{};
};

}   // namespace examples::base
