#include "SceneManager.hpp"

#include <algorithm>

namespace engine::scene_graph {

auto SceneManager::add_scene(Scene&& t_scene) noexcept -> SceneManager&
{
    m_scenes.push_back(std::move(t_scene));

    return *this;
}

auto SceneManager::load_scene(Scene::Id t_scene_id, int t_priority) noexcept
    -> Result
{
    auto scene_iter{ std::ranges::find(m_scenes, t_scene_id, &Scene::id) };

    if (scene_iter == m_scenes.end()) {
        return Result::eFailure;
    }

    scene_iter->load();

    auto scene_priority_iter{ std::upper_bound(
        m_active_scene_priorities.begin(),
        m_active_scene_priorities.end(),
        t_priority,
        std::less{}
    ) };

    m_active_scenes.insert(
        m_active_scenes.begin()
            + std::distance(
                m_active_scene_priorities.begin(), scene_priority_iter
            ),
        scene_iter.operator->()
    );
    m_active_scene_priorities.insert(scene_priority_iter, t_priority);

    return Result::eSuccess;
}

auto SceneManager::unload_scene(Scene::Id t_scene_id) noexcept -> void
{
    auto scene_iter{ std::ranges::find(m_scenes, t_scene_id, &Scene::id) };
    if (scene_iter == m_scenes.end()) {
        return;
    }

    scene_iter->unload();

    auto active_scene_iter{
        std::ranges::find(m_active_scenes, scene_iter.operator->())
    };

    m_active_scenes.erase(active_scene_iter);
    m_active_scene_priorities.erase(
        m_active_scene_priorities.begin()
        + std::distance(m_active_scenes.begin(), active_scene_iter)
    );
}

auto SceneManager::active_scenes() noexcept -> std::vector<Scene*>&
{
    return m_active_scenes;
}

}   // namespace engine::scene_graph
