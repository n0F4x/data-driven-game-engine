module;

#include <chrono>

#include <glm/glm.hpp>

export module examples.base.Controller;

import ddge.deprecated.gfx.Camera;
import ddge.deprecated.window.Key;
import ddge.deprecated.window.Window;

namespace examples::base {

export class Controller {
public:
    explicit Controller(float movement_speed) noexcept;

    auto update(
        const ddge::window::Window&   window,
        std::chrono::duration<double> delta_time
    ) -> void;

    [[nodiscard]]
    auto update_camera(ddge::gfx::Camera camera) const noexcept -> ddge::gfx::Camera;

private:
    struct MovementKeyMap {
        ddge::window::Key left     = ddge::window::eA;
        ddge::window::Key right    = ddge::window::eD;
        ddge::window::Key forward  = ddge::window::eW;
        ddge::window::Key backward = ddge::window::eS;
        ddge::window::Key up       = ddge::window::eSpace;
        ddge::window::Key down     = ddge::window::eLeftShift;
    } m_movement_key_map;

    double m_movement_speed{ 5.f };
    double m_mouse_speed{ 0.002f };

    glm::dvec3 m_position{};
    double     m_horizontal_angle{};
    double     m_vertical_angle{};
};

}   // namespace examples::base
