#include "SceneManager.hpp"

#include <spdlog/spdlog.h>

namespace engine::scene_graph {

auto SceneManager::add_scene(Scene&& t_scene) noexcept -> SceneManager&
{
    m_scenes.push_back(std::move(t_scene));

    return *this;
}

auto SceneManager::activate_scene(Scene::Id t_scene_id, int t_priority) noexcept
    -> Result
{
    const auto scene_iter{
        std::ranges::find(m_scenes, t_scene_id, &Scene::id)
    };

    if (scene_iter == m_scenes.cend()) {
        SPDLOG_ERROR("There is no scene with the following id: {}", t_scene_id);
        return Result::eFailure;
    }

    const auto scene_priority_iter{ std::upper_bound(
        m_active_scene_priorities.cbegin(),
        m_active_scene_priorities.cend(),
        t_priority,
        std::less{}
    ) };

    m_active_scenes.insert(
        m_active_scenes.cbegin()
            + std::distance(
                m_active_scene_priorities.cbegin(), scene_priority_iter
            ),
        scene_iter.operator->()
    );
    m_active_scene_priorities.insert(scene_priority_iter, t_priority);

    return Result::eSuccess;
}

auto SceneManager::deactivate_scene(Scene::Id t_scene_id) noexcept -> void
{
    const auto scene_iter{
        std::ranges::find(std::as_const(m_scenes), t_scene_id, &Scene::id)
    };
    if (scene_iter == m_scenes.cend()) {
        return;
    }

    const auto active_scene_iter{ std::ranges::find(
        std::as_const(m_active_scenes), scene_iter.operator->()
    ) };

    m_active_scenes.erase(active_scene_iter);
    m_active_scene_priorities.erase(
        m_active_scene_priorities.cbegin()
        + std::distance(m_active_scenes.cbegin(), active_scene_iter)
    );
}

auto SceneManager::change_priority(Scene::Id t_scene_id, int t_new_priority)
    -> void
{
    deactivate_scene(t_scene_id);
    activate_scene(t_scene_id, t_new_priority);
}

auto SceneManager::active_scenes() noexcept -> std::vector<Scene*>&
{
    return m_active_scenes;
}

}   // namespace engine::scene_graph
