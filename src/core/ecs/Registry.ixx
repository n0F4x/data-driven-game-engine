module;

#include <any>
#include <cassert>
#include <set>
#include <span>
#include <unordered_map>
#include <utility>

#ifdef ENGINE_ENABLE_TESTS
  #include <catch2/catch_test_macros.hpp>
#endif

export module core.ecs.Registry;

import utility.containers.Any;
import utility.containers.SlotMultiMap;
import utility.containers.SlotMap;
import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.reflection.hash;
import utility.meta.type_traits.all_different;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_is_sorted;
import utility.meta.type_traits.type_list.type_list_sort;
import utility.meta.type_traits.type_list.type_list_to;
import utility.all_same;
import utility.hashing;
import utility.ScopeGuard;
import utility.TypeList;

import core.ecs.Archetype;
import core.ecs.Component;
import core.ecs.Entity;

template <typename T>
concept decays_to_component_c = core::ecs::component_c<std::decay_t<T>>;

struct Entity {
    core::ecs::ArchetypeID archetype_id;
    core::ecs::Key         key;
};

namespace core::ecs {

// TODO: constexpr when std::unordered_map becomes constexpr
// TODO: remove emptied archetypes (this also means more exception guarantees upon create)
class Registry {
public:
    template <::decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
    auto create(Components_T&&... components) -> ID;

    template <component_c... Components_T, typename Self_T>
    [[nodiscard]]
    auto get(this Self_T&&, ID id)
        -> std::tuple<::util::meta::forward_like_t<Components_T, Self_T>...>;

    auto destroy(ID id) -> bool;

private:
    std::unordered_map<
        ComponentID::Underlying,
        std::unordered_map<ArchetypeID::Underlying, ErasedComponentContainer>>
                                                           m_component_tables;
    std::unordered_map<ArchetypeID::Underlying, Archetype> m_archetypes;
    util::SparseMap<ID::Underlying, ::Entity>              m_entities;

    template <typename Self_T>
    [[nodiscard]]
    auto entity(this Self_T&&, ID id) -> ::util::meta::forward_like_t<::Entity, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto archetype(this Self_T&&, ArchetypeID archetype_id)
        -> ::util::meta::forward_like_t<Archetype, Self_T>;

    [[nodiscard]]
    auto index(ArchetypeID archetype_id, Key key) -> Index;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto component_container(this Self_T&& self, ArchetypeID archetype_id)
        -> ::util::meta::forward_like_t<ComponentContainer<Component_T>, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto erased_component_container(
        this Self_T&& self,
        ArchetypeID   archetype_id,
        ComponentID   component_id
    ) -> ::util::meta::forward_like_t<ErasedComponentContainer, Self_T>;

    template <typename Component_T, typename... Args_T>
    [[nodiscard]]
    auto emplace_component(
        ArchetypeID archetype_id,
        std::in_place_type_t<Component_T>,
        Args_T&&... args
    ) -> Index;

    auto erase_components(ArchetypeID archetype_id, const Archetype& archetype, Index index)
        -> void;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto component(this Self_T&& self, ArchetypeID archetype_id, Index index)
        -> ::util::meta::forward_like_t<Component_T, Self_T>;
};

}   // namespace core::ecs

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
auto core::ecs::Registry::create(Components_T&&... components) -> ID
{
    constexpr static ArchetypeID archetype_id{
        archetype_id_v<std::decay_t<Components_T>...>
    };

    Archetype& archetype = m_archetypes
                               .try_emplace(
                                   archetype_id.underlying(),
                                   util::TypeList<std::decay_t<Components_T>...>{}
                               )
                               .first->second;

    const auto [key, index] = archetype.emplace();

    [index]<std::same_as<Index>... Indices_T>(const Indices_T... indices) {
        assert((indices == index) && ...);
    }(emplace_component(
        archetype_id,
        std::in_place_type<std::decay_t<Components_T>>,
        std::forward<Components_T>(components)
    )...);

    return ID{
        m_entities.emplace(Entity{ .archetype_id = archetype_id, .key = Key{ key } })
    };
}

template <core::ecs::component_c... Components_T, typename Self_T>
auto core::ecs::Registry::get(this Self_T&& self, const ID id)
    -> std::tuple<util::meta::forward_like_t<Components_T, Self_T>...>
{
    const auto [archetype_id, key]{ self.entity(id) };

    const Index index{ self.index(archetype_id, key) };

    return std::forward_as_tuple(
        std::forward_like<Self_T>(
            self.template component<Components_T>(archetype_id, index)
        )...
    );
}

auto core::ecs::Registry::destroy(const ID id) -> bool
{
    return m_entities.erase(id.underlying())
        .transform([this](const Entity entity) {
            const auto [archetype_id, key]{ entity };

            Archetype& archetype{ this->archetype(archetype_id) };

            const Index index = *archetype.erase(key);

            erase_components(archetype_id, archetype, index);

            if (archetype.empty()) {
                m_archetypes.erase(m_archetypes.find(archetype_id.underlying()));
            }

            return true;
        })
        .value_or(false);
}

template <typename Self_T>
auto core::ecs::Registry::entity(this Self_T&& self, const ID id)
    -> util::meta::forward_like_t<::Entity, Self_T>
{
    return std::forward_like<Self_T>(self.m_entities.get(id.underlying()));
}

template <typename Self_T>
auto core::ecs::Registry::archetype(this Self_T&& self, const ArchetypeID archetype_id)
    -> util::meta::forward_like_t<Archetype, Self_T>
{
    const auto arhetype_iter{ self.m_archetypes.find(archetype_id.underlying()) };
    assert(arhetype_iter != self.m_archetypes.cend());
    return std::forward_like<Self_T>(arhetype_iter->second);
}

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::component_container(this Self_T&& self, ArchetypeID archetype_id)
    -> util::meta::forward_like_t<ComponentContainer<Component_T>, Self_T>
{
    const auto component_containers_iter{
        self.m_component_tables.find(component_id_v<Component_T>.underlying())
    };
    assert(component_containers_iter != self.m_component_tables.cend());

    const auto component_container_iter{
        component_containers_iter->second.find(archetype_id.underlying())
    };
    assert(component_container_iter != component_containers_iter->second.cend());

    return std::forward_like<Self_T>(
        component_container_iter->second.template get<ComponentContainer<Component_T>>()
    );
}

template <typename Self_T>
auto core::ecs::Registry::erased_component_container(
    this Self_T&&     self,
    const ArchetypeID archetype_id,
    const ComponentID component_id
) -> util::meta::forward_like_t<ErasedComponentContainer, Self_T>
{
    const auto components_container_iter{
        self.m_component_tables.find(component_id.underlying())
    };
    assert(components_container_iter != self.m_component_tables.cend());

    const auto component_container_iter{
        components_container_iter->second.find(archetype_id.underlying())
    };
    assert(component_container_iter != components_container_iter->second.cend());

    return std::forward_like<Self_T>(component_container_iter->second);
}

template <typename Component_T, typename... Args_T>
auto core::ecs::Registry::emplace_component(
    const ArchetypeID archetype_id,
    std::in_place_type_t<Component_T>,
    Args_T&&... args
) -> Index
{
    using ComponentContainer = ComponentContainer<std::decay_t<Component_T>>;

    constexpr static ComponentID component_id{ component_id_v<Component_T> };

    ComponentContainer& component_vector =
        m_component_tables[component_id.underlying()]
            .try_emplace(archetype_id.underlying(), component_tag<Component_T>)
            .first->second.template get<ComponentContainer>();

    const Index index{ static_cast<Index::Underlying>(component_vector.size()) };
    component_vector.push_back(std::forward<Args_T>(args)...);

    return index;
}

auto core::ecs::Registry::index(const ArchetypeID archetype_id, const Key key) -> Index
{
    return archetype(archetype_id).get(key);
}

auto core::ecs::Registry::erase_components(
    const ArchetypeID archetype_id,
    const Archetype&  archetype,
    const Index       index
) -> void
{
    for (const ComponentID component_id : archetype.component_set()) {
        ErasedComponentContainer& component_container{
            erased_component_container(archetype_id, component_id)
        };

        component_container.erase(index);

        if (component_container.empty()) {
            const auto component_table_iter{
                m_component_tables.find(component_id.underlying())
            };
            assert(component_table_iter != m_component_tables.cend());

            const auto component_container_iter{
                component_table_iter->second.find(archetype_id.underlying())
            };
            assert(component_container_iter != component_table_iter->second.cend());
            component_table_iter->second.erase(component_container_iter);

            if (component_table_iter->second.empty()) {
                m_component_tables.erase(component_table_iter);
            }
        }
    }
}

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::component(
    this Self_T&&     self,
    const ArchetypeID archetype_id,
    const Index       index
) -> util::meta::forward_like_t<Component_T, Self_T>
{
    auto&& component_container{
        self.template component_container<Component_T>(archetype_id)
    };
    return std::forward_like<Self_T>(component_container[index.underlying()]);
}

module :private;

#ifdef ENGINE_ENABLE_TESTS

TEST_CASE("core::ecs::Registry")
{
    core::ecs::Registry registry;

    SECTION("create")
    {
        decltype(auto) id = registry.create(int{}, float{});

        static_assert(std::is_same_v<decltype(id), core::ecs::ID>);
    }

    SECTION("get")
    {
        constexpr static int   integer{ 1 };
        constexpr static float floating{ 2 };

        const auto id = registry.create(integer, floating);

        decltype(auto) integer_tuple{ registry.get<int>(id) };
        static_assert(std::is_same_v<decltype(integer_tuple), std::tuple<int&>>);
        REQUIRE(std::get<int&>(integer_tuple) == integer);

        decltype(auto) floating_tuple{ registry.get<float>(id) };
        static_assert(std::is_same_v<decltype(floating_tuple), std::tuple<float&>>);
        REQUIRE(std::get<float&>(floating_tuple) == floating);

        decltype(auto) combined_tuple{ registry.get<int, float>(id) };
        static_assert(std::is_same_v<decltype(combined_tuple), std::tuple<int&, float&>>);
        REQUIRE(combined_tuple == std::make_tuple(integer, floating));

        decltype(auto) shuffled_tuple{ registry.get<float, int>(id) };
        static_assert(std::is_same_v<decltype(shuffled_tuple), std::tuple<float&, int&>>);
        REQUIRE(shuffled_tuple == std::make_tuple(floating, integer));
    }

    SECTION("destroy contained")
    {
        const auto id      = registry.create(int{}, float{});
        const bool success = registry.destroy(id);

        REQUIRE(success);
    }

    SECTION("destroy non-contained")
    {
        const bool success = registry.destroy(core::ecs::ID{});

        REQUIRE(!success);
    }
}

#endif
