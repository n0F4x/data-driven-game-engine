module;

#include <any>
#include <cassert>
#include <set>
#include <span>
#include <unordered_map>
#include <utility>

export module core.ecs:Registry;

import utility.containers.Any;
import utility.containers.SlotMultiMap;
import utility.containers.SlotMap;
import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.all_different;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_is_sorted;
import utility.meta.type_traits.type_list.type_list_sort;
import utility.meta.type_traits.type_list.type_list_to;
import utility.all_same;
import utility.hashing;
import utility.ScopeGuard;
import utility.TypeList;

import :Archetype;
import :Component;
import :Entity;
import :specialization_of_registry_c;

template <typename T>
concept decays_to_component_c = core::ecs::component_c<std::decay_t<T>>;

template <core::ecs::specialization_of_registry_c Registry_T>
struct Entity {
    ArchetypeID archetype_id;
    Key         key;
};

namespace core::ecs {

// TODO: constexpr when std::unordered_map becomes constexpr
// TODO: remove emptied archetypes (this also means more exception guarantees upon create)
export template <auto tag_T = [] {}>
class Registry {
public:
    template <::decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
    auto create(Components_T&&... components) -> ID<Registry>;

    template <component_c... Components_T, typename Self_T>
    [[nodiscard]]
    auto get(this Self_T&&, ID<Registry> id)
        -> std::tuple<::util::meta::forward_like_t<Components_T, Self_T>...>;

    auto destroy(ID<Registry> id) -> bool;

private:
    std::unordered_map<
        ComponentID,
        std::unordered_map<::ArchetypeID, ::ErasedComponentContainer<Registry>>>
                                                    m_component_tables;
    std::unordered_map<::ArchetypeID, ::Archetype>  m_archetypes;
    util::SlotMap<ID<Registry>, ::Entity<Registry>> m_entities;

    template <typename Self_T>
    [[nodiscard]]
    auto entity(this Self_T&&, ID<Registry> id)
        -> ::util::meta::forward_like_t<::Entity<Registry>, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto archetype(this Self_T&&, ::ArchetypeID archetype_id)
        -> ::util::meta::forward_like_t<::Archetype, Self_T>;

    [[nodiscard]]
    auto index(::ArchetypeID archetype_id, ::Key key) -> ::Index;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto component_container(this Self_T&& self, ::ArchetypeID archetype_id)
        -> ::util::meta::forward_like_t<::ComponentContainer<Component_T>, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto erased_component_container(
        this Self_T&& self,
        ArchetypeID   archetype_id,
        ComponentID   component_id
    ) -> ::util::meta::forward_like_t<::ErasedComponentContainer<Registry>, Self_T>;

    template <typename Component_T, typename... Args_T>
    [[nodiscard]]
    auto emplace_component(
        ::ArchetypeID archetype_id,
        std::in_place_type_t<Component_T>,
        Args_T&&... args
    ) -> ::Index;

    auto erase_components(
        ::ArchetypeID      archetype_id,
        const ::Archetype& archetype,
        ::Index            index
    ) -> void;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto component(this Self_T&& self, ::ArchetypeID archetype_id, ::Index index)
        -> ::util::meta::forward_like_t<Component_T, Self_T>;
};

}   // namespace core::ecs

template <auto tag_T>
template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
auto core::ecs::Registry<tag_T>::create(Components_T&&... components) -> ID<Registry>
{
    constexpr static ArchetypeID archetype_id{
        archetype_id_v<Registry, std::decay_t<Components_T>...>
    };

    Archetype& archetype =
        m_archetypes
            .try_emplace(
                archetype_id,
                util::ValueSequence<
                    component_id_v<Registry, std::decay_t<Components_T>>.underlying()...>{}
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

    return ID<Registry>{ m_entities.emplace(
        ::Entity<Registry>{ .archetype_id = archetype_id, .key = ::Key{ key } }
    ) };
}

template <auto tag_T>
template <core::ecs::component_c... Components_T, typename Self_T>
auto core::ecs::Registry<tag_T>::get(this Self_T&& self, const ID<Registry> id)
    -> std::tuple<util::meta::forward_like_t<Components_T, Self_T>...>
{
    const auto [archetype_id, key]{ self.entity(id) };

    const ::Index index{ self.index(archetype_id, key) };

    return std::forward_as_tuple(
        std::forward_like<Self_T>(
            self.template component<Components_T>(archetype_id, index)
        )...
    );
}

template <auto tag_T>
auto core::ecs::Registry<tag_T>::destroy(const ID<Registry> id) -> bool
{
    return m_entities.erase(id)
        .transform([this](const ::Entity<Registry> entity) {
            const auto [archetype_id, key]{ entity };

            Archetype& archetype{ this->archetype(archetype_id) };

            const Index index = *archetype.erase(key);

            erase_components(archetype_id, archetype, index);

            if (archetype.empty()) {
                m_archetypes.erase(m_archetypes.find(archetype_id));
            }

            return true;
        })
        .value_or(false);
}

template <auto tag_T>
template <typename Self_T>
auto core::ecs::Registry<tag_T>::entity(this Self_T&& self, const ID<Registry> id)
    -> util::meta::forward_like_t<::Entity<Registry>, Self_T>
{
    return std::forward_like<Self_T>(self.m_entities.get(id));
}

template <auto tag_T>
template <typename Self_T>
auto core::ecs::Registry<tag_T>::archetype(
    this Self_T&&     self,
    const ArchetypeID archetype_id
) -> util::meta::forward_like_t<Archetype, Self_T>
{
    const auto arhetype_iter{ self.m_archetypes.find(archetype_id) };
    assert(arhetype_iter != self.m_archetypes.cend());
    return std::forward_like<Self_T>(arhetype_iter->second);
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::component_container(
    this Self_T&& self,
    ArchetypeID   archetype_id
) -> util::meta::forward_like_t<ComponentContainer<Component_T>, Self_T>
{
    const auto component_containers_iter{
        self.m_component_tables.find(component_id_v<Registry, Component_T>)
    };
    assert(component_containers_iter != self.m_component_tables.cend());

    const auto component_container_iter{
        component_containers_iter->second.find(archetype_id)
    };
    assert(component_container_iter != component_containers_iter->second.cend());

    return std::forward_like<Self_T>(
        component_container_iter->second.template get<ComponentContainer<Component_T>>()
    );
}

template <auto tag_T>
template <typename Self_T>
auto core::ecs::Registry<tag_T>::erased_component_container(
    this Self_T&&     self,
    const ArchetypeID archetype_id,
    const ComponentID component_id
) -> util::meta::forward_like_t<ErasedComponentContainer<Registry>, Self_T>
{
    const auto components_container_iter{ self.m_component_tables.find(component_id) };
    assert(components_container_iter != self.m_component_tables.cend());

    const auto component_container_iter{
        components_container_iter->second.find(archetype_id)
    };
    assert(component_container_iter != components_container_iter->second.cend());

    return std::forward_like<Self_T>(component_container_iter->second);
}

template <auto tag_T>
template <typename Component_T, typename... Args_T>
auto core::ecs::Registry<tag_T>::emplace_component(
    const ArchetypeID archetype_id,
    std::in_place_type_t<Component_T>,
    Args_T&&... args
) -> Index
{
    using ComponentContainer = ComponentContainer<std::decay_t<Component_T>>;

    constexpr static ComponentID component_id{ component_id_v<Registry, Component_T> };

    ComponentContainer& component_vector =
        m_component_tables[component_id]
            .try_emplace(archetype_id, component_tag<Component_T>)
            .first->second.template get<ComponentContainer>();

    const Index index{ static_cast<Index::Underlying>(component_vector.size()) };
    component_vector.push_back(std::forward<Args_T>(args)...);

    return index;
}

template <auto tag_T>
auto core::ecs::Registry<tag_T>::index(const ArchetypeID archetype_id, const Key key)
    -> Index
{
    return archetype(archetype_id).get(key);
}

template <auto tag_T>
auto core::ecs::Registry<tag_T>::erase_components(
    const ArchetypeID archetype_id,
    const Archetype&  archetype,
    const Index       index
) -> void
{
    for (const ComponentID component_id : archetype.component_set()) {
        ErasedComponentContainer<Registry>& component_container{
            erased_component_container(archetype_id, component_id)
        };

        component_container.erase(index);

        if (component_container.empty()) {
            const auto component_table_iter{ m_component_tables.find(component_id) };
            assert(component_table_iter != m_component_tables.cend());

            const auto component_container_iter{
                component_table_iter->second.find(archetype_id)
            };
            assert(component_container_iter != component_table_iter->second.cend());
            component_table_iter->second.erase(component_container_iter);

            if (component_table_iter->second.empty()) {
                m_component_tables.erase(component_table_iter);
            }
        }
    }
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::component(
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
