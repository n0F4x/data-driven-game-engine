#include "Scene.hpp"

namespace engine::scene_graph {

Scene::Scene(const entt::hashed_string&& t_id) noexcept : m_id{ t_id.value() }
{}

auto Scene::id() const noexcept -> Scene::Id
{
    return m_id;
}

}   // namespace engine::scene_graph
