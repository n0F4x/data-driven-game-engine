#pragma once

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

namespace engine::scene_graph {

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

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Id             m_id;
    entt::registry m_world;
};

}   // namespace engine::scene_graph
