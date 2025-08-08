module;

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
#include <utility>

#include "utility/contracts_macros.hpp"

export module modules.ecs:Registry;

import utility.containers.Any;
import utility.containers.OptionalRef;
import utility.containers.SlotMultiMap;
import utility.containers.SlotMap;
import utility.meta.algorithms.enumerate;
import utility.meta.concepts.all_different;
import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.type_list.type_list_is_sorted;
import utility.meta.type_traits.type_list.type_list_sort;
import utility.meta.type_traits.type_list.type_list_to;
import utility.meta.type_traits.const_like;
import utility.all_same;
import utility.hashing;
import utility.ScopeGuard;
import utility.TypeList;

import :Archetype;
import :ArchetypeID;
import :component_c;
import :ComponentID;
import :ComponentTableMap;
import :ComponentTableMap.extensions;
import :decays_to_component_c;
import :ID;
import :LookupTable;
import :LookupTableMap;
import :query.QueryClosure.fwd;
import :RecordIndex;
import :RecordID;

struct Entity {
    ArchetypeID archetype_id;
    RecordID    record_id;
};

namespace modules::ecs {

// TODO: constexpr when containers becomes constexpr
// TODO: revise exception guarantees
export class Registry {
public:
    constexpr static modules::ecs::ID null_id{
        std::numeric_limits<modules::ecs::ID::Underlying>::max()
    };

    template <::decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0)
             && util::meta::all_different_c<std::decay_t<Components_T>...>
    auto create(Components_T&&... components) -> modules::ecs::ID;

    auto destroy(modules::ecs::ID id) -> bool;

    template <component_c... Components_T, typename Self_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto get(this Self_T&&, modules::ecs::ID id) -> std::
        tuple<util::meta::const_like_t<Components_T, std::remove_reference_t<Self_T>>&...>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto get_single(this Self_T&&, modules::ecs::ID id)
        -> util::meta::const_like_t<Component_T, std::remove_reference_t<Self_T>>&;

    template <component_c... Components_T, typename Self_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto find(this Self_T&&, modules::ecs::ID id) noexcept -> std::tuple<::util::OptionalRef<
        util::meta::const_like_t<Components_T, std::remove_reference_t<Self_T>>>...>;

    template <component_c... Components_T, typename Self_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto find_all(this Self_T&&, modules::ecs::ID id) noexcept -> std::optional<std::tuple<
        util::meta::const_like_t<Components_T, std::remove_reference_t<Self_T>>&...>>;

    template <component_c Component_T, typename Self_T>
    [[nodiscard]]
    auto find_single(this Self_T&&, modules::ecs::ID id) noexcept -> ::util::
        OptionalRef<util::meta::const_like_t<Component_T, std::remove_reference_t<Self_T>>>;

    template <component_c... Components_T>
        requires util::meta::all_different_c<Components_T...>
    [[nodiscard]]
    auto contains_all(modules::ecs::ID id) const noexcept -> bool;

    template <::decays_to_component_c... Components_T>
        requires util::meta::all_different_c<std::decay_t<Components_T>...>
    auto insert(modules::ecs::ID id, Components_T&&... components) -> void;

    template <::decays_to_component_c... Components_T>
        requires util::meta::all_different_c<std::decay_t<Components_T>...>
    auto insert_or_replace(modules::ecs::ID id, Components_T&&... components) -> void;

    template <component_c... Components_T>
        requires util::meta::all_different_c<Components_T...>
    auto remove(modules::ecs::ID id) -> std::tuple<Components_T...>;

    template <component_c Component_T>
    auto remove_single(modules::ecs::ID id) -> Component_T;

    template <component_c... Components_T>
        requires util::meta::all_different_c<Components_T...>
    auto erase(modules::ecs::ID id) -> std::tuple<std::optional<Components_T>...>;

    template <component_c... Components_T>
        requires util::meta::all_different_c<Components_T...>
    auto erase_all(modules::ecs::ID id) -> std::optional<std::tuple<Components_T...>>;

    template <component_c Component_T>
    auto erase_single(modules::ecs::ID id) -> std::optional<Component_T>;

    [[nodiscard]]
    auto exists(modules::ecs::ID id) const noexcept -> bool;

private:
    template <query_parameter_c... Parameters_T>
        requires ::query_parameter_components_are_all_different_c<Parameters_T...>
    friend class Query;


    ::ComponentTableMap                                m_component_tables;
    ::LookupTableMap                                   m_lookup_tables;
    util::SlotMap<modules::ecs::ID::Underlying, ::Entity> m_entities;


    template <typename Self_T>
    [[nodiscard]]
    auto get_entity(this Self_T&&, modules::ecs::ID id)
        -> util::meta::const_like_t<::Entity, std::remove_reference_t<Self_T>>&;

    template <typename Self_T>
    [[nodiscard]]
    auto find_entity(this Self_T&&, modules::ecs::ID id) -> ::util::
        OptionalRef<util::meta::const_like_t<::Entity, std::remove_reference_t<Self_T>>>;


    template <decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0)
             && util::meta::all_different_c<std::decay_t<Components_T>...>
    [[nodiscard]]
    auto insert(ID id, Entity entity, Components_T&&... components) -> Entity;

    template <decays_to_component_c... Components_T>
        requires(sizeof...(Components_T) > 0)
             && util::meta::all_different_c<std::decay_t<Components_T>...>
    auto forced_insert_or_replace(
        ID                       id,
        const Entity&            entity,
        LookupTableMap::Iterator lookup_tables_iterator,
        RecordIndex              record_index,
        Components_T&&... components
    ) -> Entity;

    template <component_c... Components_T>
        requires(sizeof...(Components_T) > 0)
             && util::meta::all_different_c<Components_T...>
    auto remove(modules::ecs::ID id, Entity& entity) -> std::tuple<Components_T...>;
};

}   // namespace modules::ecs

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto modules::ecs::Registry::create(Components_T&&... components) -> modules::ecs::ID
{
    modules::ecs::ID id{ m_entities.next_key() };

    const ArchetypeID archetype_id{ archetype_from<std::decay_t<Components_T>...>() };


    const auto [record_id, record_index] = m_lookup_tables.insert(id, archetype_id);

    [[maybe_unused]]
    const RecordIndex component_record_index = ::insert(
        m_component_tables, archetype_id, std::forward<Components_T>(components)...
    );
    assert(component_record_index == record_index);

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

auto modules::ecs::Registry::destroy(const modules::ecs::ID id) -> bool
{
    return m_entities.erase(id.underlying())
        .transform([](const auto entity_and_index) {
            return std::get<Entity>(entity_and_index);
        })
        .transform([this, id](const ::Entity entity) {
            std::ignore = id;

            const auto [archetype_id, record_id]{ entity };

            const auto [removed_id, record_index] =
                m_lookup_tables.remove(archetype_id, record_id);
            assert(removed_id == id);

            ::remove_components(
                m_component_tables,
                archetype_id->sorted_component_ids(),
                archetype_id,
                record_index
            );

            return true;
        })
        .value_or(false);
}

template <modules::ecs::component_c... Components_T, typename Self_T>
    requires util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::get(this Self_T&& self, const modules::ecs::ID id) -> std::
    tuple<util::meta::const_like_t<Components_T, std::remove_reference_t<Self_T>>&...>
{
    PRECOND(self.exists(id));
    PRECOND((self.template contains_all<Components_T...>(id)));

    const auto [archetype_id, record_id]{ self.get_entity(id) };

    return std::forward_as_tuple(
        ::get_component<Components_T>(
            self.m_component_tables,
            archetype_id,
            self.m_lookup_tables.get_record_index(archetype_id, record_id)
        )...
    );
}

template <modules::ecs::component_c Component_T, typename Self_T>
auto modules::ecs::Registry::get_single(this Self_T&& self, const modules::ecs::ID id)
    -> util::meta::const_like_t<Component_T, std::remove_reference_t<Self_T>>&
{
    PRECOND(self.exists(id));
    PRECOND((self.template contains_all<Component_T>(id)));

    const auto [archetype_id, record_id]{ self.get_entity(id) };

    return ::get_component<Component_T>(
        self.m_component_tables,
        archetype_id,
        self.m_lookup_tables.get_record_index(archetype_id, record_id)
    );
}

template <modules::ecs::component_c... Components_T, typename Self_T>
    requires util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::find(this Self_T&& self, const modules::ecs::ID id) noexcept
    -> std::tuple<util::OptionalRef<
        util::meta::const_like_t<Components_T, std::remove_reference_t<Self_T>>>...>
{
    const auto optional_entity = self.find_entity(id);
    if (!optional_entity.has_value()) {
        return {};
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    const LookupTable& lookup_table{ self.m_lookup_tables.get_lookup_table(archetype_id) };

    return std::forward_as_tuple(
        ::find_component<Components_T>(
            self.m_component_tables, archetype_id, lookup_table.get(record_id)
        )...
    );
}

template <modules::ecs::component_c... Components_T, typename Self_T>
    requires util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::find_all(this Self_T&& self, const modules::ecs::ID id) noexcept
    -> std::optional<std::tuple<
        util::meta::const_like_t<Components_T, std::remove_reference_t<Self_T>>&...>>
{
    const auto optional_entity = self.find_entity(id);
    if (!optional_entity.has_value()) {
        return std::nullopt;
    }

    const auto [archetype_id, record_id]{ *optional_entity };
    if (!archetype_id->template contains_all_of_components<Components_T...>()) {
        return std::nullopt;
    }

    const ::LookupTable& lookup_table{
        self.m_lookup_tables.get_lookup_table(archetype_id)
    };

    return std::forward_as_tuple(
        ::get_component<Components_T>(
            self.m_component_tables, archetype_id, lookup_table.get(record_id)
        )...
    );
}

template <modules::ecs::component_c Component_T, typename Self_T>
auto modules::ecs::Registry::find_single(this Self_T&& self, const modules::ecs::ID id) noexcept
    -> util::
        OptionalRef<util::meta::const_like_t<Component_T, std::remove_reference_t<Self_T>>>
{
    const auto optional_entity = self.find_entity(id);
    if (!optional_entity.has_value()) {
        return std::nullopt;
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    return ::find_component<Component_T>(
        self.m_component_tables,
        archetype_id,
        self.m_lookup_tables.get_record_index(archetype_id, record_id)
    );
}

template <modules::ecs::component_c... Components_T>
    requires util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::contains_all(const modules::ecs::ID id) const noexcept -> bool
{
    const auto optional_entity = find_entity(id);
    if (!optional_entity.has_value()) {
        return false;
    }

    const auto [archetype_id, record_id]{ *optional_entity };

    assert(m_lookup_tables.contains(archetype_id));

    return archetype_id->contains_all_of_components<Components_T...>();
}

template <::decays_to_component_c... Components_T>
    requires util::meta::all_different_c<std::decay_t<Components_T>...>
auto modules::ecs::Registry::insert(const modules::ecs::ID id, Components_T&&... components)
    -> void
{
    PRECOND(exists(id));

    if constexpr (sizeof...(Components_T) > 0) {
        auto& entity{ get_entity(id) };

        entity = insert(id, entity, std::forward<Components_T>(components)...);
    }
}

template <::decays_to_component_c... Components_T>
    requires util::meta::all_different_c<std::decay_t<Components_T>...>
auto modules::ecs::Registry::insert_or_replace(
    const modules::ecs::ID id,
    Components_T&&... components
) -> void
{
    PRECOND(exists(id));

    if constexpr (sizeof...(Components_T) > 0) {
        auto& entity{ get_entity(id) };

        const auto lookup_tables_iterator{
            m_lookup_tables.get_iterator(entity.archetype_id)
        };

        const RecordIndex record_index{
            lookup_tables_iterator->second.get(entity.record_id)
        };

        if (entity.archetype_id->contains_all_of_components<Components_T...>()) {
            replace_components(
                m_component_tables,
                entity.archetype_id,
                record_index,
                std::forward<Components_T>(components)...
            );
        }
        else {
            entity = forced_insert_or_replace(
                id,
                entity,
                lookup_tables_iterator,
                record_index,
                std::forward<Components_T>(components)...
            );
        }
    }
}

template <modules::ecs::component_c... Components_T>
    requires util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::remove(const modules::ecs::ID id) -> std::tuple<Components_T...>
{
    PRECOND(exists(id));
    PRECOND((contains_all<Components_T...>(id)));

    if constexpr (sizeof...(Components_T) == 0) {
        return {};
    }
    else {
        return remove<Components_T...>(id, get_entity(id));
    }
}

template <modules::ecs::component_c Component_T>
auto modules::ecs::Registry::remove_single(const modules::ecs::ID id) -> Component_T
{
    PRECOND(exists(id));
    PRECOND((contains_all<Component_T>(id)));

    return std::get<0>(remove<Component_T>(id, get_entity(id)));
}

template <modules::ecs::component_c... Components_T>
    requires util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::erase(const modules::ecs::ID id)
    -> std::tuple<std::optional<Components_T>...>
{
    if constexpr (sizeof...(Components_T) == 0) {
        return {};
    }
    else {
        return find_entity(id)
            .transform(
                [this,
                 id](::Entity& entity) -> std::tuple<std::optional<Components_T>...> {
                    const auto [archetype_id, record_id]{ entity };

                    const std::ranges::view auto new_component_ids{
                        archetype_id->sorted_component_ids()
                        | std::views::filter([](const ComponentID component_id) {
                              return !std::ranges::binary_search(
                                  component_id_set_from<Components_T...>(), component_id
                              );
                          })
                    };

                    const ArchetypeID new_archetype_id{
                        archetype_from(new_component_ids)
                    };

                    if (archetype_id == new_archetype_id) {
                        return {};
                    }


                    const auto [removed_id, record_index] =
                        m_lookup_tables.remove(archetype_id, record_id);
                    assert(removed_id == id);

                    const RecordID new_record_id =
                        std::get<RecordID>(m_lookup_tables.insert(id, new_archetype_id));


                    std::tuple<std::optional<Components_T>...> result = std::make_tuple(
                        [this, archetype_id, record_index]<
                            typename Component_T>(std::type_identity<Component_T>)
                            -> std::optional<Component_T> {
                            if (!archetype_id->contains_all_of_components<Component_T>())
                            {
                                return std::nullopt;
                            }

                            return m_component_tables.remove_component<Component_T>(
                                archetype_id, record_index
                            );
                        }(std::type_identity<Components_T>{})...
                    );

                    move_components(
                        m_component_tables,
                        new_component_ids,
                        archetype_id,
                        record_index,
                        new_archetype_id
                    );


                    entity = ::Entity{
                        .archetype_id = new_archetype_id,
                        .record_id    = new_record_id,
                    };


                    return result;
                }
            )
            .value_or(std::tuple<std::optional<Components_T>...>{});
    }
}

template <modules::ecs::component_c... Components_T>
    requires util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::erase_all(const modules::ecs::ID id)
    -> std::optional<std::tuple<Components_T...>>
{
    if constexpr (sizeof...(Components_T) == 0) {
        return std::optional{ std::tuple<Components_T...>{} };
    }
    else {
        return find_entity(id).and_then(
            [this, id](::Entity& entity) -> std::optional<std::tuple<Components_T...>> {
                if (!entity.archetype_id->contains_all_of_components<Components_T...>()) {
                    return std::nullopt;
                }

                return remove<Components_T...>(id, entity);
            }
        );
    }
}

template <modules::ecs::component_c Component_T>
auto modules::ecs::Registry::erase_single(modules::ecs::ID id) -> std::optional<Component_T>
{
    return find_entity(id).and_then(
        [this, id](::Entity& entity) -> std::optional<Component_T> {
            if (!entity.archetype_id->contains_all_of_components<Component_T>()) {
                return std::nullopt;
            }

            return std::get<0>(remove<Component_T>(id, entity));
        }
    );
}

auto modules::ecs::Registry::exists(const modules::ecs::ID id) const noexcept -> bool
{
    return m_entities.contains(id.underlying());
}

template <typename Self_T>
auto modules::ecs::Registry::get_entity(this Self_T&& self, const modules::ecs::ID id)
    -> util::meta::const_like_t<::Entity, std::remove_reference_t<Self_T>>&
{
    return self.m_entities.get(id.underlying());
}

template <typename Self_T>
auto modules::ecs::Registry::find_entity(
    this Self_T&&       self,
    const modules::ecs::ID id
) -> util::OptionalRef<util::meta::const_like_t<::Entity, std::remove_reference_t<Self_T>>>
{
    return self.m_entities.find(id.underlying());
}

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto modules::ecs::Registry::insert(
    const ID     id,
    const Entity entity,
    Components_T&&... components
) -> Entity
{
    PRECOND(
        entity.archetype_id->contains_none_of_components<std::decay_t<Components_T>...>()
    );

    const ArchetypeID new_archetype_id{ archetype_from<std::decay_t<Components_T>...>(
        entity.archetype_id->sorted_component_ids()
    ) };


    const auto [removed_id, record_index]{
        m_lookup_tables.remove(entity.archetype_id, entity.record_id)
    };
    assert(removed_id == id);

    const auto [new_record_id, new_record_index]{
        m_lookup_tables.insert(id, new_archetype_id)
    };


    ::move_components(
        m_component_tables,
        entity.archetype_id->sorted_component_ids(),
        entity.archetype_id,
        record_index,
        new_archetype_id
    );

    [[maybe_unused]]
    const RecordIndex component_record_index = ::insert(
        m_component_tables, new_archetype_id, std::forward<Components_T>(components)...
    );
    assert(component_record_index == new_record_index);

    return Entity{ .archetype_id = new_archetype_id, .record_id = new_record_id };
}

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto modules::ecs::Registry::forced_insert_or_replace(
    const ID                       id,
    const Entity&                  entity,
    const LookupTableMap::Iterator lookup_tables_iterator,
    const RecordIndex              record_index,
    Components_T&&... components
) -> Entity
{
    const auto [archetype_id, record_id]{ entity };

    const ArchetypeID new_archetype_id{
        archetype_from<Components_T...>(archetype_id->sorted_component_ids())
    };


    [[maybe_unused]] const auto [removed_id, removed_record_index] =
        m_lookup_tables.remove(lookup_tables_iterator, record_id);
    assert(removed_id == id);
    assert(removed_record_index == record_index);

    const auto [new_record_id, new_record_index] =
        m_lookup_tables.insert(id, new_archetype_id);


    ::move_components(
        m_component_tables,
        archetype_id->sorted_component_ids(),
        archetype_id,
        record_index,
        new_archetype_id
    );

    insert_each_component_if(
        m_component_tables,
        [archetype_id]<typename Component_T> {
            return !archetype_id->contains_all_of_components<Component_T>();
        },
        new_archetype_id,
        std::forward<Components_T>(components)...
    );


    return Entity{ .archetype_id = new_archetype_id, .record_id = new_record_id };
}

template <modules::ecs::component_c... Components_T>
    requires(sizeof...(Components_T) > 0) && util::meta::all_different_c<Components_T...>
auto modules::ecs::Registry::remove(const modules::ecs::ID id, Entity& entity)
    -> std::tuple<Components_T...>
{
    const auto [archetype_id, record_id]{ entity };
    PRECOND(archetype_id->contains_all_of_components<Components_T...>());

    std::ranges::view auto new_component_id_set{
        archetype_id->sorted_component_ids()
        | std::views::filter([](const ComponentID component_id) {
              return !std::ranges::binary_search(
                  component_id_set_from<Components_T...>(), component_id
              );
          })
    };

    const ArchetypeID new_archetype_id{ archetype_from(new_component_id_set) };

    const auto [removed_id, record_index] =
        m_lookup_tables.remove(archetype_id, record_id);
    assert(removed_id == id);

    const RecordID new_record_id =
        std::get<RecordID>(m_lookup_tables.insert(id, new_archetype_id));


    std::tuple<Components_T...> result = ::remove_components<Components_T...>(
        m_component_tables, archetype_id, record_index
    );

    move_components(
        m_component_tables,
        new_component_id_set,
        archetype_id,
        record_index,
        new_archetype_id
    );


    entity = ::Entity{ .archetype_id = new_archetype_id, .record_id = new_record_id };


    return result;
}
