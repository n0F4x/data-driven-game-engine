module;

#include <algorithm>
#include <cassert>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <span>
#include <unordered_map>
#include <utility>

#include "utility/contracts.hpp"

export module core.ecs:Registry;

import utility.containers.Any;
import utility.containers.OptionalRef;
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
import :ArchetypeID;
import :component_c;
import :ComponentContainer;
import :ComponentID;
import :ComponentTable;
import :ComponentTag;
import :ErasedComponentContainer;
import :Query.fwd;
import :RecordIndex;
import :RecordID;
export import :ID;

struct Entity {
    ArchetypeID archetype_id;
    RecordID    record_id;
};

namespace core::ecs {

// TODO: constexpr when std::unordered_map becomes constexpr
// TODO: remove emptied archetypes (this also means more exception guarantees upon create)
export template <auto tag_T = [] {}>
class Registry {
public:
    constexpr static ID<Registry> null_id{
        std::numeric_limits<typename ID<Registry>::Underlying>::max()
    };

    template <::decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
    auto create(Components_T&&... components) -> ID<Registry>;

    auto destroy(ID<Registry> id) -> bool;

    template <component_c... Components_T, typename Self_T>
        requires(util::meta::all_different_v<Components_T...>)
    [[nodiscard]]
    auto get(this Self_T&&, ID<Registry> id)
        -> std::tuple<::util::meta::forward_like_t<Components_T, Self_T>...>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto get_single(this Self_T&&, ID<Registry> id)
        -> ::util::meta::forward_like_t<Component_T, Self_T>;

    template <component_c... Components_T, typename Self_T>
        requires(util::meta::all_different_v<Components_T...>)
    [[nodiscard]]
    auto find(this Self_T&&, ID<Registry> id) noexcept -> std::tuple<::util::OptionalRef<
        std::remove_reference_t<::util::meta::forward_like_t<Components_T, Self_T>>>...>;

    template <component_c... Components_T, typename Self_T>
        requires(util::meta::all_different_v<Components_T...>)
    [[nodiscard]]
    auto find_all(this Self_T&&, ID<Registry> id) noexcept
        -> std::optional<std::tuple<::util::meta::forward_like_t<Components_T, Self_T>...>>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto find_single(this Self_T&&, ID<Registry> id) noexcept -> ::util::OptionalRef<
        std::remove_reference_t<::util::meta::forward_like_t<Component_T, Self_T>>>;

    template <component_c... Components_T>
        requires(util::meta::all_different_v<Components_T...>)
    [[nodiscard]]
    auto contains_all(ID<Registry> id) const noexcept -> bool;

private:
    template <typename... Components_T>
        requires(queryable_component_c<std::remove_const_t<Components_T>> && ...)
             && (::util::meta::all_different_v<std::remove_const_t<Components_T>...>)
    friend class Query;

    std::unordered_map<::ComponentID, ComponentTable>  m_component_tables;
    std::map<::ArchetypeID, std::vector<ID<Registry>>> m_entity_table;
    std::unordered_map<::ArchetypeID, ::Archetype>     m_archetypes;
    util::SlotMap<ID<Registry>, ::Entity>              m_entities;

    template <typename Self_T>
    [[nodiscard]]
    auto get_entity(this Self_T&&, ID<Registry> id)
        -> ::util::meta::forward_like_t<::Entity, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto find_entity(this Self_T&&, ID<Registry> id) -> ::util::
        OptionalRef<std::remove_reference_t<util::meta::forward_like_t<Entity, Self_T>>>;

    [[nodiscard]]
    auto get_record_index(::ArchetypeID archetype_id, ::RecordID record_id) const
        -> ::RecordIndex;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto get_component_container(this Self_T&& self, ::ArchetypeID archetype_id)
        -> ::util::meta::forward_like_t<::ComponentContainer<Component_T>, Self_T>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto find_component_container(this Self_T&& self, ::ArchetypeID archetype_id)
        -> ::util::OptionalRef<std::remove_reference_t<
            ::util::meta::forward_like_t<::ComponentContainer<Component_T>, Self_T>>>;

    template <typename Component_T, typename... Args_T>
    [[nodiscard]]
    auto emplace_component(
        ::ArchetypeID archetype_id,
        std::in_place_type_t<Component_T>,
        Args_T&&... args
    ) -> ::RecordIndex;

    auto remove_components(
        ::ArchetypeID                archetype_id,
        std::span<const ComponentID> component_id_set,
        ::RecordIndex                record_index
    ) -> void;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto get_component(
        this Self_T&& self,
        ::ArchetypeID archetype_id,
        ::RecordIndex record_index
    ) -> ::util::meta::forward_like_t<Component_T, Self_T>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto find_component(
        this Self_T&& self,
        ::ArchetypeID archetype_id,
        ::RecordIndex record_index
    )
        -> ::util::OptionalRef<
            std::remove_reference_t<::util::meta::forward_like_t<Component_T, Self_T>>>;
};

}   // namespace core::ecs

template <auto tag_T>
template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0, util::meta::all_different_v<std::decay_t<Components_T>...>)
auto core::ecs::Registry<tag_T>::create(Components_T&&... components) -> ID<Registry>
{
    constexpr static ArchetypeID archetype_id{
        ::archetype_id<std::decay_t<Components_T>...>
    };

    Archetype& archetype =
        m_archetypes
            .try_emplace(
                archetype_id,
                util::ValueSequence<
                    ComponentID::Underlying,
                    component_id<std::decay_t<Components_T>>.underlying()...>{}
            )
            .first->second;

    const auto [record_id, record_index] = archetype.emplace();

    [record_index]<std::same_as<RecordIndex>... Indices_T>(
        const Indices_T... record_indices
    ) { assert((record_indices == record_index) && ...); }(emplace_component(
        archetype_id,
        std::in_place_type<std::decay_t<Components_T>>,
        std::forward<Components_T>(components)
    )...);

    return ID<Registry>{ m_entities.emplace(
        ::Entity{ .archetype_id = archetype_id, .record_id = ::RecordID{ record_id } }
    ) };
}

template <auto tag_T>
auto core::ecs::Registry<tag_T>::destroy(const ID<Registry> id) -> bool
{
    return m_entities.erase(id)
        .transform([this](const ::Entity entity) {
            const auto [archetype_id, record_id]{ entity };

            const auto arhetypes_iter{ m_archetypes.find(archetype_id) };
            assert(arhetypes_iter != m_archetypes.cend());

            const RecordIndex record_index = *arhetypes_iter->second.erase(record_id);

            remove_components(
                archetype_id, arhetypes_iter->second.component_id_set(), record_index
            );

            if (arhetypes_iter->second.empty()) {
                [[maybe_unused]]
                const auto extracted_archetype_node = m_archetypes.extract(arhetypes_iter);
                assert(!extracted_archetype_node.empty());
            }

            return true;
        })
        .value_or(false);
}

template <auto tag_T>
template <core::ecs::component_c... Components_T, typename Self_T>
    requires(util::meta::all_different_v<Components_T...>)
auto core::ecs::Registry<tag_T>::get(this Self_T&& self, const ID<Registry> id)
    -> std::tuple<util::meta::forward_like_t<Components_T, Self_T>...>
{
    const auto [archetype_id, record_id]{ self.get_entity(id) };

    const ::RecordIndex record_index{ self.get_record_index(archetype_id, record_id) };

    return std::forward_as_tuple(
        std::forward_like<Self_T>(
            self.template get_component<Components_T>(archetype_id, record_index)
        )...
    );
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::get_single(this Self_T&& self, const ID<Registry> id)
    -> util::meta::forward_like_t<Component_T, Self_T>
{
    const auto [archetype_id, record_id]{ self.get_entity(id) };

    const ::RecordIndex record_index{ self.get_record_index(archetype_id, record_id) };

    return std::forward_like<Self_T>(
        self.template get_component<Component_T>(archetype_id, record_index)
    );
}

template <auto tag_T>
template <core::ecs::component_c... Components_T, typename Self_T>
    requires(util::meta::all_different_v<Components_T...>)
auto core::ecs::Registry<tag_T>::find(this Self_T&& self, const ID<Registry> id) noexcept
    -> std::tuple<util::OptionalRef<
        std::remove_reference_t<util::meta::forward_like_t<Components_T, Self_T>>>...>
{
    const auto optional_entity = self.find_entity(id);
    if (!optional_entity.has_value()) {
        return std::tuple<util::OptionalRef<std::remove_reference_t<
            util::meta::forward_like_t<Components_T, Self_T>>>...>{};
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    const auto arhetypes_iter{ self.m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != self.m_archetypes.cend());

    const ::Archetype& archetype{ arhetypes_iter->second };

    return std::forward_as_tuple(
        std::forward_like<Self_T>(self.template find_component<Components_T>(
            archetype_id, archetype.get(record_id)
        ))...
    );
}

template <auto tag_T>
template <core::ecs::component_c... Components_T, typename Self_T>
    requires(util::meta::all_different_v<Components_T...>)
auto core::ecs::Registry<tag_T>::find_all(this Self_T&& self, const ID<Registry> id) noexcept
    -> std::optional<std::tuple<util::meta::forward_like_t<Components_T, Self_T>...>>
{
    const auto optional_entity = self.find_entity(id);
    if (!optional_entity.has_value()) {
        return std::nullopt;
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    const auto arhetypes_iter{ self.m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != self.m_archetypes.cend());

    const ::Archetype& archetype{ arhetypes_iter->second };
    if (!archetype.contains_components<Components_T...>()) {
        return std::nullopt;
    }

    return std::forward_as_tuple(
        std::forward_like<Self_T>(self.template get_component<Components_T>(
            archetype_id, archetype.get(record_id)
        ))...
    );
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::find_single(
    this Self_T&&      self,
    const ID<Registry> id
) noexcept -> util::
    OptionalRef<std::remove_reference_t<util::meta::forward_like_t<Component_T, Self_T>>>
{
    const auto optional_entity = self.find_entity(id);
    if (!optional_entity.has_value()) {
        return std::nullopt;
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    const auto arhetypes_iter{ self.m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != self.m_archetypes.cend());

    const ::Archetype& archetype{ arhetypes_iter->second };

    return std::forward_like<Self_T>(
        self.template find_component<Component_T>(archetype_id, archetype.get(record_id))
    );
}

template <auto tag_T>
template <core::ecs::component_c... Components_T>
    requires(util::meta::all_different_v<Components_T...>)
auto core::ecs::Registry<tag_T>::contains_all(const ID<Registry> id) const noexcept
    -> bool
{
    const auto optional_entity = find_entity(id);
    if (!optional_entity.has_value()) {
        return false;
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    const auto arhetypes_iter{ m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != m_archetypes.cend());

    const ::Archetype& archetype{ arhetypes_iter->second };

    return archetype.contains_components<Components_T...>();
}

template <auto tag_T>
template <typename Self_T>
auto core::ecs::Registry<tag_T>::get_entity(this Self_T&& self, const ID<Registry> id)
    -> util::meta::forward_like_t<::Entity, Self_T>
{
    return std::forward_like<Self_T>(self.m_entities.get(id));
}

template <auto tag_T>
template <typename Self_T>
auto core::ecs::Registry<tag_T>::find_entity(this Self_T&& self, const ID<Registry> id)
    -> ::util::
        OptionalRef<std::remove_reference_t<util::meta::forward_like_t<Entity, Self_T>>>
{
    return std::forward<Self_T>(self).m_entities.find(id);
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::get_component_container(
    this Self_T&& self,
    ArchetypeID   archetype_id
) -> util::meta::forward_like_t<ComponentContainer<Component_T>, Self_T>
{
    const auto component_containers_iter{
        self.m_component_tables.find(component_id<Component_T>)
    };
    PRECOND(component_containers_iter != self.m_component_tables.cend());

    const auto component_container_iter{
        component_containers_iter->second.find(archetype_id)
    };
    PRECOND(component_container_iter != component_containers_iter->second.cend());

    return std::forward_like<Self_T>(
        component_container_iter->second.template get<ComponentContainer<Component_T>>()
    );
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::find_component_container(
    this Self_T&&     self,
    const ArchetypeID archetype_id
)
    -> util::OptionalRef<std::remove_reference_t<
        util::meta::forward_like_t<ComponentContainer<Component_T>, Self_T>>>
{
    const auto component_containers_iter{
        self.m_component_tables.find(component_id<Component_T>)
    };
    if (component_containers_iter == self.m_component_tables.cend()) {
        return std::nullopt;
    }

    const auto component_container_iter{
        component_containers_iter->second.find(archetype_id)
    };
    if (component_container_iter == component_containers_iter->second.cend()) {
        return std::nullopt;
    }

    return ::util::OptionalRef{ std::forward_like<Self_T>(
        component_container_iter->second.template get<ComponentContainer<Component_T>>()
    ) };
}

template <auto tag_T>
template <typename Component_T, typename... Args_T>
auto core::ecs::Registry<tag_T>::emplace_component(
    const ArchetypeID archetype_id,
    std::in_place_type_t<Component_T>,
    Args_T&&... args
) -> RecordIndex
{
    using ComponentContainer = ComponentContainer<std::decay_t<Component_T>>;

    constexpr static ComponentID component_id{ ::component_id<Component_T> };

    ComponentContainer& component_vector =
        m_component_tables[component_id]
            .try_emplace(archetype_id, component_tag<Component_T>)
            .first->second.template get<ComponentContainer>();

    const RecordIndex record_index{
        static_cast<RecordIndex::Underlying>(component_vector.size())
    };
    component_vector.push_back(std::forward<Args_T>(args)...);

    return record_index;
}

template <auto tag_T>
auto core::ecs::Registry<tag_T>::get_record_index(
    const ArchetypeID archetype_id,
    const RecordID    record_id
) const -> RecordIndex
{
    const auto arhetypes_iter{ m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != m_archetypes.cend());
    return arhetypes_iter->second.get(record_id);
}

template <auto tag_T>
auto core::ecs::Registry<tag_T>::remove_components(
    const ArchetypeID                  archetype_id,
    const std::span<const ComponentID> component_id_set,
    const RecordIndex                  record_index
) -> void
{
    for (const ComponentID component_id : component_id_set) {
        const auto component_tables_container_iter{
            m_component_tables.find(component_id)
        };
        PRECOND(component_tables_container_iter != m_component_tables.cend());

        const auto component_containers_iter{
            component_tables_container_iter->second.find(archetype_id)
        };
        PRECOND(
            component_containers_iter != component_tables_container_iter->second.cend()
        );

        [[maybe_unused]]
        const bool could_erase_component =
            component_containers_iter->second.erase(record_index);
        PRECOND(could_erase_component);

        if (component_containers_iter->second.empty()) {
            [[maybe_unused]]
            const auto component_container_node =
                component_tables_container_iter->second.extract(component_containers_iter);
            assert(!component_container_node.empty());
        }
    }
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::get_component(
    this Self_T&&     self,
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> util::meta::forward_like_t<Component_T, Self_T>
{
    auto&& component_container{
        self.template get_component_container<Component_T>(archetype_id)
    };
    return std::forward_like<Self_T>(component_container[record_index.underlying()]);
}

template <auto tag_T>
template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry<tag_T>::find_component(
    this Self_T&&     self,
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> util::
    OptionalRef<std::remove_reference_t<util::meta::forward_like_t<Component_T, Self_T>>>
{
    return std::forward<Self_T>(self)
        .template find_component_container<Component_T>(archetype_id)
        .transform(
            [record_index]<typename ComponentContainer_T>(
                ComponentContainer_T&& component_container
            ) -> decltype(auto) {
                return std::forward<ComponentContainer_T>(component_container
                )[record_index.underlying()];
            }
        );
}
