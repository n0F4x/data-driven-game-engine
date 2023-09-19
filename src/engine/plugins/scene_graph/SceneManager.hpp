#pragma once

#include <memory>
#include <vector>

#include "engine/utility/Result.hpp"

#include "Scene.hpp"

namespace engine::scene_graph {

class SceneManager {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    SceneManager()                        = default;
    SceneManager(const SceneManager&)     = delete;
    SceneManager(SceneManager&&) noexcept = default;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    auto add_scene(Scene&& t_scene) noexcept -> void;

    auto load_scene(Scene::Id t_scene_id, int t_priority) noexcept -> Result;
    auto unload_scene(Scene::Id t_scene_id) noexcept -> void;

    [[nodiscard]] auto active_scenes() noexcept -> std::vector<Scene*>&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    std::vector<Scene>  m_scenes;
    std::vector<Scene*> m_active_scenes;
    std::vector<int>    m_active_scene_priorities;
};

}   // namespace engine::scene_graph
