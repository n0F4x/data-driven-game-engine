#pragma once

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/sigh.hpp>

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

    auto load() noexcept -> void;
    auto unload() noexcept -> void;

    [[nodiscard]] auto loaded_sink() noexcept
        -> entt::sink<entt::sigh<void(Scene&)>>&;
    [[nodiscard]] auto unloaded_sink() noexcept
        -> entt::sink<entt::sigh<void(Scene&)>>&;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    Id     m_id;
    World  m_world;
    Entity m_root;

    entt::sigh<void(Scene&)>              m_loaded_signal;
    entt::sink<decltype(m_loaded_signal)> m_loaded_sink{ m_loaded_signal };

    entt::sigh<void(Scene&)>                m_unloaded_signal;
    entt::sink<decltype(m_unloaded_signal)> m_unloaded_sink{
        m_unloaded_signal
    };
};

}   // namespace engine::scene_graph
