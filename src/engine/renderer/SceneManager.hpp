#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Scene.hpp"

namespace engine::renderer {

class SceneManager {
public:
    auto emplace_scene(size_t t_id, auto&&... t_args) -> Scene&;

private:
    std::vector<size_t> m_ids;
    std::vector<Scene>  m_scenes;
};

}   // namespace engine::renderer

#include "SceneManager.inl"
