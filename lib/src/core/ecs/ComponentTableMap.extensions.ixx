module;

#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

export module core.ecs:ComponentTableMap.extensions;

import utility.meta.algorithms.enumerate;
import utility.meta.concepts.all_different;
import utility.meta.concepts.decays_to;
import utility.meta.concepts.ranges.input_range_of;
import utility.meta.type_traits.const_like;
import utility.all_same;

import :ArchetypeID;
import :component_c;
import :ComponentTable.extensions;
import :ComponentTableMap;
import :decays_to_component_c;
import :RecordIndex;

inline namespace extensions {

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto insert(ComponentTableMap& it, ArchetypeID archetype_id, Components_T&&... components)
    -> RecordIndex;

template <util::meta::input_range_of_c<ComponentID> ComponentIDInputRange>
auto remove_components(
    ComponentTableMap&    it,
    ComponentIDInputRange component_ids,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> void;

template <core::ecs::component_c... Components_T>
    requires util::meta::all_different_c<Components_T...>
auto remove_components(
    ComponentTableMap& it,
    ArchetypeID        archetype_id,
    RecordIndex        record_index
) -> std::tuple<Components_T...>;

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto replace_components(
    ComponentTableMap& it,
    ArchetypeID        archetype_id,
    RecordIndex        record_index,
    Components_T&&... components
) -> void;

template <decays_to_component_c Component_T>
auto replace_component(
    ComponentTableMap& it,
    ArchetypeID        archetype_id,
    RecordIndex        record_index,
    Component_T&&      component
) -> void;

template <typename Predicate_T, decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto insert_each_component_if(
    ComponentTableMap& it,
    Predicate_T        predicate,
    ArchetypeID        archetype_id,
    Components_T&&... components
) -> void;

template <util::meta::input_range_of_c<ComponentID> ComponentIDInputRange>
auto move_components(
    ComponentTableMap&    it,
    ComponentIDInputRange component_ids,
    ArchetypeID           archetype_id,
    RecordIndex           record_index,
    ArchetypeID           new_archetype_id
) -> void;

template <
    core::ecs::component_c                     Component_T,
    util::meta::decays_to_c<ComponentTableMap> ComponentTableMap_T>
auto get_component(
    ComponentTableMap_T&& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTableMap_T>>&;

template <
    core::ecs::component_c                     Component_T,
    util::meta::decays_to_c<ComponentTableMap> ComponentTableMap_T>
auto find_component(
    ComponentTableMap_T&& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
)
    -> util::OptionalRef<
        util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTableMap_T>>>;

}   // namespace extensions

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto extensions::insert(
    ComponentTableMap& it,
    const ArchetypeID  archetype_id,
    Components_T&&... components
) -> RecordIndex
{
    return []<std::same_as<RecordIndex>... Indices_T>(const Indices_T... record_indices) {
        assert(util::all_same(record_indices...));
        return record_indices...[0];
    }(it.insert(archetype_id, std::forward<Components_T>(components))...);
}

template <util::meta::input_range_of_c<ComponentID> ComponentIDInputRange>
auto extensions::remove_components(
    ComponentTableMap&    it,
    ComponentIDInputRange component_ids,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
) -> void
{
    for (const ComponentID component_id : component_ids) {
        it.remove_component(archetype_id, record_index, component_id);
    }
}

template <core::ecs::component_c... Components_T>
    requires util::meta::all_different_c<Components_T...>
auto extensions::remove_components(
    ComponentTableMap& it,
    const ArchetypeID  archetype_id,
    const RecordIndex  record_index
) -> std::tuple<Components_T...>
{
    return std::make_tuple(it.remove_component<Components_T>(archetype_id, record_index)...
    );
}

template <decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto extensions::replace_components(
    ComponentTableMap& it,
    const ArchetypeID  archetype_id,
    const RecordIndex  record_index,
    Components_T&&... components
) -> void
{
    (replace_component(
         it, archetype_id, record_index, std::forward<Components_T>(components)
     ),
     ...);
}

template <decays_to_component_c Component_T>
auto extensions::replace_component(
    ComponentTableMap& it,
    const ArchetypeID  archetype_id,
    const RecordIndex  record_index,
    Component_T&&      component
) -> void
{
    get_component<Component_T>(it, archetype_id, record_index) =
        std::forward<Component_T>(component);
}

template <typename Predicate_T, decays_to_component_c... Components_T>
    requires(sizeof...(Components_T) > 0)
         && util::meta::all_different_c<std::decay_t<Components_T>...>
auto extensions::insert_each_component_if(
    ComponentTableMap& it,
    Predicate_T        predicate,
    const ArchetypeID  archetype_id,
    Components_T&&... components
) -> void
{
    util::meta::enumerate<util::TypeList<Components_T...>>(
        [&it,
         &predicate,
         archetype_id,
         &components...]<size_t index_T, typename Component_T> {
            if (predicate.template operator()<Component_T>()) {
                it.insert(archetype_id, std::forward<Component_T>(components...[index_T]));
            }
        }
    );
}

template <util::meta::input_range_of_c<ComponentID> ComponentIDInputRange>
auto extensions::move_components(
    ComponentTableMap&    it,
    ComponentIDInputRange component_ids,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index,
    const ArchetypeID     new_archetype_id
) -> void
{
    for (const ComponentID component_id : component_ids) {
        it.move_component(component_id, archetype_id, record_index, new_archetype_id);
    }
}

template <
    core::ecs::component_c                     Component_T,
    util::meta::decays_to_c<ComponentTableMap> ComponentTableMap_T>
auto extensions::get_component(
    ComponentTableMap_T&& it,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
) -> util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTableMap_T>>&
{
    return get_component<Component_T>(
        it.template get_component_table<Component_T>(), archetype_id, record_index
    );
}

template <
    core::ecs::component_c                     Component_T,
    util::meta::decays_to_c<ComponentTableMap> ComponentTableMap_T>
auto extensions::find_component(
    ComponentTableMap_T&& it,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
)
    -> util::OptionalRef<
        util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTableMap_T>>>
{
    return it.template find_component_table<Component_T>().and_then(
        [archetype_id, record_index](auto& component_table) {
            return find_component<Component_T>(
                component_table, archetype_id, record_index
            );
        }
    );
}
