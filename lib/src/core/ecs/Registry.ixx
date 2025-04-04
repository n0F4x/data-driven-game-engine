module;

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <span>
#include <utility>

#include "utility/contracts.hpp"

export module core.ecs:Registry;

import utility.containers.Any;
import utility.containers.OptionalRef;
import utility.containers.SlotMultiMap;
import utility.containers.SlotMap;
import utility.meta.algorithms.enumerate;
import utility.meta.concepts.all_different;
import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
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
import :ArchetypeTable;
import :component_c;
import :ComponentContainer;
import :ComponentID;
import :ComponentTable;
import :ComponentTag;
import :ErasedComponentContainer;
import :ID;
import :LookupTable;
import :query.QueryClosure.fwd;
import :RecordIndex;
import :RecordID;

struct Entity {
    ArchetypeID archetype_id;
    RecordID    record_id;
};

namespace core::ecs {

// TODO: constexpr when containers becomes constexpr
// TODO: remove emptied archetypes (this also means more exception guarantees upon create)
export class Registry {
public:
    constexpr static core::ecs::ID null_id{
        std::numeric_limits<core::ecs::ID::Underlying>::max()
    };

    template <::decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0)
             && util::meta::all_different_c<std::decay_t<Components_T>...>
    auto create(Components_T&&... components) -> core::ecs::ID;

    auto destroy(core::ecs::ID id) -> bool;

    template <component_c... Components_T, typename Self_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto get(this Self_T&&, core::ecs::ID id)
        -> std::tuple<::util::meta::forward_like_t<Components_T, Self_T>...>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto get_single(this Self_T&&, core::ecs::ID id)
        -> ::util::meta::forward_like_t<Component_T, Self_T>;

    template <component_c... Components_T, typename Self_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto find(this Self_T&&, core::ecs::ID id) noexcept -> std::tuple<::util::OptionalRef<
        std::remove_reference_t<::util::meta::forward_like_t<Components_T, Self_T>>>...>;

    template <component_c... Components_T, typename Self_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto find_all(this Self_T&&, core::ecs::ID id) noexcept
        -> std::optional<std::tuple<::util::meta::forward_like_t<Components_T, Self_T>...>>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto find_single(this Self_T&&, core::ecs::ID id) noexcept -> ::util::OptionalRef<
        std::remove_reference_t<::util::meta::forward_like_t<Component_T, Self_T>>>;

    template <component_c... Components_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto contains_all(core::ecs::ID id) const noexcept -> bool;

    template <::decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0)
             && util::meta::all_different_c<std::decay_t<Components_T>...>
    auto insert(core::ecs::ID id, Components_T&&... components) -> void;

    template <::decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0)
             && util::meta::all_different_c<std::decay_t<Components_T>...>
    auto insert_or_replace(core::ecs::ID id, Components_T&&... components) -> void;

private:
    template <query_parameter_c... Parameters_T>
        requires ::query_parameter_components_are_all_different_c<Parameters_T...>
    friend struct ::QueryClosure;


    std::map<::ComponentID, ::ComponentTable>          m_component_tables;
    ::ArchetypeTable                                   m_archetypes;
    util::SlotMap<core::ecs::ID::Underlying, ::Entity> m_entities;


    template <component_c Component_T, typename... Args_T>
    auto emplace_component(::ArchetypeID archetype_id, Args_T&&... args) -> ::RecordIndex;

    template <decays_to_component_c Component_T>
    auto insert_component(::ArchetypeID archetype_id, Component_T&& component)
        -> ::RecordIndex;

    auto remove_components(::ArchetypeID archetype_id, ::RecordIndex record_index)
        -> void;

    auto remove_component(
        ::ArchetypeID archetype_id,
        ::RecordIndex record_index,
        ComponentID   component_id
    ) -> void;

    auto remove_component(
        ::ArchetypeID archetype_id,
        ::RecordIndex record_index,
        std::remove_reference_t<decltype(m_component_tables)>::iterator
            component_tables_iterator
    ) -> void;

    auto remove_component(
        ::RecordIndex record_index,
        std::remove_reference_t<decltype(m_component_tables)>::iterator
            component_tables_iterator,
        std::remove_reference_t<decltype(m_component_tables)>::value_type::second_type::
            iterator component_containers_iterator
    ) -> void;

    template <decays_to_component_c... Components_T>
    auto replace_components(
        ArchetypeID archetype_id,
        RecordIndex record_index,
        Components_T&&... components
    ) -> void;

    template <decays_to_component_c Component_T>
    auto replace_component(
        ArchetypeID   archetype_id,
        RecordIndex   record_index,
        Component_T&& component
    ) -> void;

    template <typename Predicate_T, decays_to_component_c... Components_T>
    auto insert_each_component_if(
        Predicate_T predicate,
        ArchetypeID archetype_id,
        Components_T&&... components
    ) -> void;

    auto move_components(
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ArchetypeID new_archetype_id
    ) -> void;

    auto move_component(
        ComponentID component_id,
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ArchetypeID new_archetype_id
    ) -> void;

    static auto move_component(
        std::map<::ComponentID, ::ComponentTable>::iterator component_tables_iterator,
        ArchetypeID                                         archetype_id,
        RecordIndex                                         record_index,
        ArchetypeID                                         new_archetype_id
    ) -> void;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto get_component_container(this Self_T&& self, ::ArchetypeID archetype_id)
        -> ::util::meta::forward_like_t<::ComponentContainer<Component_T>, Self_T>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto find_component_container(this Self_T&& self, ::ArchetypeID archetype_id)
        -> ::util::OptionalRef<std::remove_reference_t<
            ::util::meta::forward_like_t<::ComponentContainer<Component_T>, Self_T>>>;

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


    [[nodiscard]]
    auto get_record_index(::ArchetypeID archetype_id, ::RecordID record_id) const
        -> ::RecordIndex;


    template <typename Self_T>
    [[nodiscard]]
    auto get_entity(this Self_T&&, core::ecs::ID id)
        -> ::util::meta::forward_like_t<::Entity, Self_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto find_entity(this Self_T&&, core::ecs::ID id) -> ::util::
        OptionalRef<std::remove_reference_t<util::meta::forward_like_t<Entity, Self_T>>>;


    template <decays_to_component_c... Components_T>
    auto insert(
        ID          id,
        ArchetypeID archetype_id,
        RecordID    record_id,
        Components_T&&... components
    ) -> ArchetypeID;

    template <decays_to_component_c... Components_T>
    auto forced_insert_or_replace(
        ID                       id,
        ArchetypeID              archetype_id,
        RecordID                 record_id,
        ArchetypeTable::iterator archetypes_iterator,
        RecordIndex              record_index,
        Components_T&&... components
    ) -> ArchetypeID;
};

}   // namespace core::ecs

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto core::ecs::Registry::create(Components_T&&... components) -> core::ecs::ID
{
    core::ecs::ID id{ m_entities.next_key() };

    const ArchetypeID archetype_id{ archetype_from<std::decay_t<Components_T>...>() };

    LookupTable& lookup_table = m_archetypes[archetype_id];

    const auto [record_id, record_index] = lookup_table.emplace(id);

    [record_index]<std::same_as<RecordIndex>... Indices_T>(
        const Indices_T... record_indices
    ) {
        assert((record_indices == record_index) && ...);
    }(emplace_component<std::decay_t<Components_T>>(
        archetype_id, std::forward<Components_T>(components)
    )...);

    [[maybe_unused]]
    const auto actual_id = m_entities
                               .emplace(
                                   ::Entity{ .archetype_id = archetype_id,
                                             .record_id    = ::RecordID{ record_id } }
                               )
                               .first;
    assert(id.underlying() == actual_id);

    return id;
}

auto core::ecs::Registry::destroy(const core::ecs::ID id) -> bool
{
    return m_entities.erase(id.underlying())
        .transform([](const auto entity_and_index) {
            return std::get<Entity>(entity_and_index);
        })
        .transform([this, id](const ::Entity entity) {
            const auto [archetype_id, record_id]{ entity };

            const auto arhetypes_iter{ m_archetypes.find(archetype_id) };
            assert(arhetypes_iter != m_archetypes.cend());

            const auto id_and_record_index = *arhetypes_iter->second.erase(record_id);
            assert(std::get<ID>(id_and_record_index) == id);
            const RecordIndex record_index{ std::get<RecordIndex>(id_and_record_index) };

            remove_components(archetype_id, record_index);

            if (arhetypes_iter->second.empty()) {
                [[maybe_unused]]
                const auto extracted_archetype_node = m_archetypes.extract(arhetypes_iter);
                assert(!extracted_archetype_node.empty());
            }

            return true;
        })
        .value_or(false);
}

template <core::ecs::component_c... Components_T, typename Self_T>
    requires util::meta::all_different_c<Components_T...>
auto core::ecs::Registry::get(this Self_T&& self, const core::ecs::ID id)
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

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::get_single(this Self_T&& self, const core::ecs::ID id)
    -> util::meta::forward_like_t<Component_T, Self_T>
{
    const auto [archetype_id, record_id]{ self.get_entity(id) };

    const ::RecordIndex record_index{ self.get_record_index(archetype_id, record_id) };

    return std::forward_like<Self_T>(
        self.template get_component<Component_T>(archetype_id, record_index)
    );
}

template <core::ecs::component_c... Components_T, typename Self_T>
    requires util::meta::all_different_c<Components_T...>
auto core::ecs::Registry::find(this Self_T&& self, const core::ecs::ID id) noexcept
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

    const ::LookupTable& lookup_table{ arhetypes_iter->second };

    return std::forward_as_tuple(
        std::forward_like<Self_T>(self.template find_component<Components_T>(
            archetype_id, lookup_table.get(record_id)
        ))...
    );
}

template <core::ecs::component_c... Components_T, typename Self_T>
    requires util::meta::all_different_c<Components_T...>
auto core::ecs::Registry::find_all(this Self_T&& self, const core::ecs::ID id) noexcept
    -> std::optional<std::tuple<util::meta::forward_like_t<Components_T, Self_T>...>>
{
    const auto optional_entity = self.find_entity(id);
    if (!optional_entity.has_value()) {
        return std::nullopt;
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    const auto arhetypes_iter{ self.m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != self.m_archetypes.cend());

    const ::LookupTable& lookup_table{ arhetypes_iter->second };
    if (!archetype_id->template contains_components<Components_T...>()) {
        return std::nullopt;
    }

    return std::forward_as_tuple(
        std::forward_like<Self_T>(self.template get_component<Components_T>(
            archetype_id, lookup_table.get(record_id)
        ))...
    );
}

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::find_single(
    this Self_T&&       self,
    const core::ecs::ID id
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

    const ::LookupTable& lookup_table{ arhetypes_iter->second };

    return std::forward_like<Self_T>(self.template find_component<Component_T>(
        archetype_id, lookup_table.get(record_id)
    ));
}

template <core::ecs::component_c... Components_T>
    requires util::meta::all_different_c<Components_T...>
auto core::ecs::Registry::contains_all(const core::ecs::ID id) const noexcept -> bool
{
    const auto optional_entity = find_entity(id);
    if (!optional_entity.has_value()) {
        return false;
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    assert(m_archetypes.contains(archetype_id));

    return archetype_id->contains_components<Components_T...>();
}

template <::decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto core::ecs::Registry::insert(const core::ecs::ID id, Components_T&&... components)
    -> void
{
    auto& [archetype_id, record_id]{ get_entity(id) };

    archetype_id =
        insert(id, archetype_id, record_id, std::forward<Components_T>(components)...);
}

template <::decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto core::ecs::Registry::insert_or_replace(
    const core::ecs::ID id,
    Components_T&&... components
) -> void
{
    auto& [archetype_id, record_id]{ get_entity(id) };

    const auto arhetypes_iter{ m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != m_archetypes.cend());

    const RecordIndex record_index{ arhetypes_iter->second.get(record_id) };

    if (archetype_id->contains_components<Components_T...>()) {
        replace_components(
            archetype_id, record_index, std::forward<Components_T>(components)...
        );
    }
    else {
        archetype_id = forced_insert_or_replace(
            id,
            archetype_id,
            record_id,
            arhetypes_iter,
            record_index,
            std::forward<Components_T>(components)...
        );
    }
}

template <typename Self_T>
auto core::ecs::Registry::get_entity(this Self_T&& self, const core::ecs::ID id)
    -> util::meta::forward_like_t<::Entity, Self_T>
{
    return std::forward_like<Self_T>(self.m_entities).get(id.underlying());
}

template <typename Self_T>
auto core::ecs::Registry::find_entity(this Self_T&& self, const core::ecs::ID id) -> ::
    util::OptionalRef<std::remove_reference_t<util::meta::forward_like_t<Entity, Self_T>>>
{
    return std::forward<Self_T>(self).m_entities.find(id.underlying());
}

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::get_component_container(
    this Self_T&& self,
    ArchetypeID   archetype_id
) -> util::meta::forward_like_t<ComponentContainer<Component_T>, Self_T>
{
    const auto component_containers_iter{
        self.m_component_tables.find(component_id_of<Component_T>())
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

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::find_component_container(
    this Self_T&&     self,
    const ArchetypeID archetype_id
)
    -> util::OptionalRef<std::remove_reference_t<
        util::meta::forward_like_t<ComponentContainer<Component_T>, Self_T>>>
{
    const auto component_containers_iter{
        self.m_component_tables.find(component_id_of<Component_T>())
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

template <core::ecs::component_c Component_T, typename... Args_T>
auto core::ecs::Registry::emplace_component(
    const ArchetypeID archetype_id,
    Args_T&&... args
) -> RecordIndex
{
    using ComponentContainer = ComponentContainer<std::decay_t<Component_T>>;

    ComponentContainer& component_vector =
        m_component_tables[::component_id_of<Component_T>()]
            .try_emplace(archetype_id, component_tag<Component_T>)
            .first->second.template get<ComponentContainer>();

    const RecordIndex record_index{
        static_cast<RecordIndex::Underlying>(component_vector.size())
    };
    component_vector.emplace_back(std::forward<Args_T>(args)...);

    return record_index;
}

template <decays_to_component_c Component_T>
auto core::ecs::Registry::insert_component(
    ArchetypeID   archetype_id,
    Component_T&& component
) -> RecordIndex
{
    using ComponentContainer = ComponentContainer<std::decay_t<Component_T>>;

    ComponentContainer& component_vector =
        m_component_tables[::component_id_of<Component_T>()]
            .try_emplace(archetype_id, component_tag<Component_T>)
            .first->second.template get<ComponentContainer>();

    const RecordIndex record_index{
        static_cast<RecordIndex::Underlying>(component_vector.size())
    };
    component_vector.push_back(std::forward<Component_T>(component));

    return record_index;
}

auto core::ecs::Registry::get_record_index(
    const ArchetypeID archetype_id,
    const RecordID    record_id
) const -> RecordIndex
{
    const auto arhetypes_iter{ m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != m_archetypes.cend());
    return arhetypes_iter->second.get(record_id);
}

auto core::ecs::Registry::remove_components(
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> void
{
    for (const ComponentID component_id : archetype_id->sorted_component_ids()) {
        remove_component(archetype_id, record_index, component_id);
    }
}

auto core::ecs::Registry::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ComponentID component_id
) -> void
{
    remove_component(archetype_id, record_index, m_component_tables.find(component_id));
}

auto core::ecs::Registry::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const std::remove_reference_t<decltype(m_component_tables)>::iterator
        component_tables_iterator
) -> void
{
    PRECOND(component_tables_iterator != m_component_tables.cend());
    remove_component(
        record_index,
        component_tables_iterator,
        component_tables_iterator->second.find(archetype_id)
    );
}

auto core::ecs::Registry::remove_component(
    const RecordIndex record_index,
    const std::remove_reference_t<decltype(m_component_tables)>::iterator
        component_tables_iterator,
    const std::remove_reference_t<decltype(m_component_tables)>::value_type::second_type::
        iterator component_containers_iterator
) -> void
{
    PRECOND(component_containers_iterator != component_tables_iterator->second.cend());

    [[maybe_unused]]
    const bool could_erase_component =
        component_containers_iterator->second.erase(record_index);
    PRECOND(could_erase_component);

    if (component_containers_iterator->second.empty()) {
        [[maybe_unused]]
        const auto component_container_node =
            component_tables_iterator->second.extract(component_containers_iterator);
        assert(!component_container_node.empty());

        if (component_tables_iterator->second.empty()) {
            [[maybe_unused]]
            const auto component_table_node =
                m_component_tables.extract(component_tables_iterator);
            assert(!component_table_node.empty());
        }
    }
}

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::get_component(
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

template <core::ecs::component_c Component_T, typename Self_T>
auto core::ecs::Registry::find_component(
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

template <decays_to_component_c... Components_T>
auto core::ecs::Registry::insert(
    const ID          id,
    const ArchetypeID archetype_id,
    const RecordID    record_id,
    Components_T&&... components
) -> ArchetypeID
{
    PRECOND(archetype_id->contains_none_of_components<Components_T...>());

    const auto arhetypes_iter{ m_archetypes.find(archetype_id) };
    assert(arhetypes_iter != m_archetypes.cend());

    const ArchetypeID new_archetype_id{
        archetype_from<Components_T...>(archetype_id->sorted_component_ids())
    };


    m_archetypes[new_archetype_id].emplace(id);

    move_components(archetype_id, arhetypes_iter->second.get(record_id), new_archetype_id);

    (insert_component(new_archetype_id, std::forward<Components_T>(components)), ...);


    [[maybe_unused]]
    const std::optional optional_id_and_record_index_tuple =
        arhetypes_iter->second.erase(record_id);
    assert(optional_id_and_record_index_tuple.has_value());
    assert(std::get<ID>(*optional_id_and_record_index_tuple) == id);

    if (arhetypes_iter->second.empty()) {
        [[maybe_unused]]
        const auto extracted_archetype_node = m_archetypes.extract(arhetypes_iter);
        assert(!extracted_archetype_node.empty());
    }

    return new_archetype_id;
}

template <decays_to_component_c... Components_T>
auto core::ecs::Registry::forced_insert_or_replace(
    const ID                       id,
    const ArchetypeID              archetype_id,
    const RecordID                 record_id,
    const ArchetypeTable::iterator archetypes_iterator,
    const RecordIndex              record_index,
    Components_T&&... components
) -> ArchetypeID
{
    const ArchetypeID new_archetype_id{
        archetype_from<Components_T...>(archetype_id->sorted_component_ids())
    };


    m_archetypes[new_archetype_id].emplace(id);

    move_components(archetype_id, record_index, new_archetype_id);

    insert_each_component_if(
        [archetype_id]<typename Component_T> {
            return !archetype_id->contains_components<Component_T>();
        },
        new_archetype_id,
        std::forward<Components_T>(components)...
    );


    [[maybe_unused]]
    const std::optional optional_id_and_record_index_tuple =
        archetypes_iterator->second.erase(record_id);
    assert(optional_id_and_record_index_tuple.has_value());
    assert(std::get<ID>(*optional_id_and_record_index_tuple) == id);

    if (archetypes_iterator->second.empty()) {
        [[maybe_unused]]
        const auto extracted_archetype_node = m_archetypes.extract(archetypes_iterator);
        assert(!extracted_archetype_node.empty());
    }


    return new_archetype_id;
}

template <typename Predicate_T, decays_to_component_c... Components_T>
auto core::ecs::Registry::insert_each_component_if(
    Predicate_T       predicate,
    const ArchetypeID archetype_id,
    Components_T&&... components
) -> void
{
    util::meta::enumerate<util::TypeList<Components_T...>>(
        [this,
         &predicate,
         archetype_id,
         &components...]<size_t index_T, typename Component_T> {
            if (predicate.template operator()<Component_T>()) {
                this->insert_component(
                    archetype_id, std::forward<Component_T>(components...[index_T])
                );
            }
        }
    );
}

template <decays_to_component_c... Components_T>
auto core::ecs::Registry::replace_components(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    Components_T&&... components
) -> void
{
    (replace_component(archetype_id, record_index, std::forward<Components_T>(components)),
     ...);
}

template <decays_to_component_c Component_T>
auto core::ecs::Registry::replace_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    Component_T&&     component
) -> void
{
    get_component<Component_T>(archetype_id, record_index) =
        std::forward<Component_T>(component);
}

auto core::ecs::Registry::move_components(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ArchetypeID new_archetype_id
) -> void
{
    for (const ComponentID component_id : archetype_id->sorted_component_ids()) {
        move_component(component_id, archetype_id, record_index, new_archetype_id);
    }
}

auto core::ecs::Registry::move_component(
    const ComponentID component_id,
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ArchetypeID new_archetype_id
) -> void
{
    const auto component_tables_iterator{ m_component_tables.find(component_id) };
    assert(component_tables_iterator != m_component_tables.cend());

    move_component(
        component_tables_iterator, archetype_id, record_index, new_archetype_id
    );
}

auto core::ecs::Registry::move_component(
    const std::map<ComponentID, ComponentTable>::iterator component_tables_iterator,
    const ArchetypeID                                     archetype_id,
    const RecordIndex                                     record_index,
    const ArchetypeID                                     new_archetype_id
) -> void
{
    const auto component_containers_iterator{
        component_tables_iterator->second.find(archetype_id)
    };
    assert(component_containers_iterator != component_tables_iterator->second.cend());

    component_containers_iterator->second.move_out_to(
        record_index, component_tables_iterator->second, new_archetype_id
    );

    if (component_containers_iterator->second.empty()) {
        component_tables_iterator->second.extract(component_containers_iterator);
    }
}
