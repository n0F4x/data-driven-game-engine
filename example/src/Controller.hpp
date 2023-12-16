#pragma once

#include <chrono>

#include <glm/glm.hpp>

#include <SFML/Window.hpp>

#include "Camera.hpp"

class Controller {
public:
    auto               update(float t_delta_time) noexcept -> void;
    [[nodiscard]] auto update_camera(Camera t_camera) noexcept -> Camera;

private:
    struct MovementKeyMap {
        sf::Keyboard::Key left     = sf::Keyboard::Key::A;
        sf::Keyboard::Key right    = sf::Keyboard::Key::D;
        sf::Keyboard::Key forward  = sf::Keyboard::Key::W;
        sf::Keyboard::Key backward = sf::Keyboard::Key::S;
        sf::Keyboard::Key up       = sf::Keyboard::Key::Space;
        sf::Keyboard::Key down     = sf::Keyboard::Key::LShift;
    } m_movement_key_map;

    struct TurningKeyMap {
        sf::Keyboard::Key left  = sf::Keyboard::Key::Left;
        sf::Keyboard::Key right = sf::Keyboard::Key::Right;
        sf::Keyboard::Key up    = sf::Keyboard::Key::Up;
        sf::Keyboard::Key down  = sf::Keyboard::Key::Down;
    } m_turning_key_map;

    float m_movement_speed{ 8.f };
    float m_turning_speed{ 1.5f };

    glm::vec3 m_translation{};
    glm::vec3 m_rotation{};
};
