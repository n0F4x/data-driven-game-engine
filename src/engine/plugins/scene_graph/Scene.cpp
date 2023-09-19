#include "Scene.hpp"

namespace engine::scene_graph {

Scene::Scene(const entt::hashed_string&& t_id) noexcept
    : m_id{ t_id.value() },
      m_root{ m_world.create() }
{}

auto Scene::id() const noexcept -> Scene::Id
{
    return m_id;
}

auto Scene::world() noexcept -> entt::registry&
{
    return m_world;
}

}   // namespace engine::scene_graph
