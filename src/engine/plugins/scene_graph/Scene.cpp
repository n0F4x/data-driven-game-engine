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

auto Scene::world() noexcept -> World&
{
    return m_world;
}

auto Scene::root() const noexcept -> Entity
{
    return m_root;
}

auto Scene::load() noexcept -> void
{
    m_loaded_signal.publish(*this);
}

auto Scene::unload() noexcept -> void
{
    m_unloaded_signal.publish(*this);
}

auto Scene::loaded_sink() noexcept -> entt::sink<entt::sigh<void(Scene&)>>&
{
    return m_loaded_sink;
}

auto Scene::unloaded_sink() noexcept -> entt::sink<entt::sigh<void(Scene&)>>&
{
    return m_unloaded_sink;
}

}   // namespace engine::scene_graph
