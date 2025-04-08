module;

#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

export module core.ecs:ComponentTableMap.extensions;

import utility.containers.Any;
import utility.meta.algorithms.enumerate;
import utility.meta.concepts.all_different;
import utility.meta.concepts.decays_to;
import utility.meta.concepts.ranges.input_range_of;
import utility.all_same;

import :ArchetypeID;
import :component_c;
import :ComponentID;
import :ErasedComponentTable.extensions;
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

template <core::ecs::component_c Component_T>
auto find_component_table(ComponentTableMap& it)
    -> util::OptionalRef<ComponentTable<Component_T>>;
template <core::ecs::component_c Component_T>
auto find_component_table(const ComponentTableMap& it)
    -> util::OptionalRef<const ComponentTable<Component_T>>;

template <core::ecs::component_c Component_T>
auto get_component(
    ComponentTableMap& it,
    ArchetypeID        archetype_id,
    RecordIndex        record_index
) -> Component_T&;
template <core::ecs::component_c Component_T>
auto get_component(
    const ComponentTableMap& it,
    ArchetypeID              archetype_id,
    RecordIndex              record_index
) -> const Component_T&;

template <core::ecs::component_c Component_T>
auto find_component(
    ComponentTableMap& it,
    ArchetypeID        archetype_id,
    RecordIndex        record_index
) -> util::OptionalRef<Component_T>;
template <core::ecs::component_c Component_T>
auto find_component(
    const ComponentTableMap& it,
    ArchetypeID              archetype_id,
    RecordIndex              record_index
) -> util::OptionalRef<const Component_T>;

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

template <core::ecs::component_c Component_T>
auto extensions::find_component_table(ComponentTableMap& it)
    -> util::OptionalRef<ComponentTable<Component_T>>
{
    return it.find_component_table(component_id_of<Component_T>())
        .transform([](ErasedComponentTable& erased_component_table) -> decltype(auto) {
            return util::any_cast<ComponentTable<Component_T>>(erased_component_table);
        });
}

template <core::ecs::component_c Component_T>
auto extensions::find_component_table(const ComponentTableMap& it)
    -> util::OptionalRef<const ComponentTable<Component_T>>
{
    return find_component_table<Component_T>(const_cast<ComponentTableMap&>(it));
}

template <core::ecs::component_c Component_T>
auto extensions::get_component(
    ComponentTableMap& it,
    const ArchetypeID  archetype_id,
    const RecordIndex  record_index
) -> Component_T&
{
    return get_component<Component_T>(
        it.get_component_table(component_id_of<Component_T>()), archetype_id, record_index
    );
}

template <core::ecs::component_c Component_T>
auto extensions::get_component(
    const ComponentTableMap& it,
    const ArchetypeID        archetype_id,
    const RecordIndex        record_index
) -> const Component_T&
{
    return get_component<Component_T>(
        const_cast<ComponentTableMap&>(it), archetype_id, record_index
    );
}

template <core::ecs::component_c Component_T>
auto extensions::find_component(
    ComponentTableMap& it,
    const ArchetypeID  archetype_id,
    const RecordIndex  record_index
) -> util::OptionalRef<Component_T>
{
    return it.find_component_table(component_id_of<Component_T>())
        .and_then([archetype_id,
                   record_index](ErasedComponentTable& erased_component_table) {
            return find_component<Component_T>(
                erased_component_table, archetype_id, record_index
            );
        });
}

template <core::ecs::component_c Component_T>
auto extensions::find_component(
    const ComponentTableMap& it,
    const ArchetypeID        archetype_id,
    const RecordIndex        record_index
) -> util::OptionalRef<const Component_T>
{
    return find_component<Component_T>(
        const_cast<ComponentTableMap&>(it), archetype_id, record_index
    );
}
