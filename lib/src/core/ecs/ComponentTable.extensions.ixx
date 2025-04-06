module;

#include <type_traits>

export module core.ecs:ComponentTable.extensions;

import utility.meta.concepts.decays_to;
import utility.meta.type_traits.const_like;
import utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentContainer;
import :ComponentTable;
import :RecordIndex;

inline namespace extensions {

template <
    core::ecs::component_c                  Component_T,
    util::meta::decays_to_c<ComponentTable> ComponentTable_T>
[[nodiscard]]
auto get_component(
    ComponentTable_T&& it,
    ArchetypeID        archetype_id,
    RecordIndex        record_index
) -> util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTable_T>>&;

template <
    core::ecs::component_c                  Component_T,
    util::meta::decays_to_c<ComponentTable> ComponentTable_T>
auto find_component(
    ComponentTable_T&& it,
    ArchetypeID        archetype_id,
    RecordIndex        record_index
)
    -> util::OptionalRef<
        util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTable_T>>>;

}   // namespace extensions

template <
    core::ecs::component_c                  Component_T,
    util::meta::decays_to_c<ComponentTable> ComponentTable_T>
auto extensions::get_component(
    ComponentTable_T&& it,
    const ArchetypeID  archetype_id,
    const RecordIndex  record_index
) -> util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTable_T>>&
{
    return it.template get_component_container<Component_T>(archetype_id).get(record_index);
}

template <
    core::ecs::component_c                  Component_T,
    util::meta::decays_to_c<ComponentTable> ComponentTable_T>
auto extensions::find_component(
    ComponentTable_T&& it,
    const ArchetypeID  archetype_id,
    const RecordIndex  record_index
)
    -> util::OptionalRef<
        util::meta::const_like_t<Component_T, std::remove_reference_t<ComponentTable_T>>>
{
    return it.template find_component_container<Component_T>(archetype_id)
        .transform([record_index](auto& component_container) -> decltype(auto) {
            return component_container.get(record_index);
        });
}
