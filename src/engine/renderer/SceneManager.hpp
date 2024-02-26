#pragma once

#include <string>
#include <utility>
#include <vector>

#include <entt/config/config.h>
#include <entt/core/hashed_string.hpp>

#include "Scene.hpp"

namespace engine::renderer {

class SceneManager {
public:
    auto emplace_scene(size_t t_id, auto&&... t_args) -> void;

private:
    std::vector<std::pair<size_t, Scene>> m_scenes;
};

}   // namespace engine::renderer

#include "SceneManager.inl"
