#pragma once

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

namespace engine::scene_graph {

using World  = entt::registry;
using Entity = entt::entity;

class Scene {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Id = entt::hashed_string::hash_type;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    explicit Scene(const entt::hashed_string&& t_id) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto id() const noexcept -> Id;
    [[nodiscard]] auto world() noexcept -> World&;
    [[nodiscard]] auto root() const noexcept -> Entity;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Id     m_id;
    World  m_world;
    Entity m_root;
};

}   // namespace engine::scene_graph
