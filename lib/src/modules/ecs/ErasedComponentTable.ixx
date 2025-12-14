module;

#include <functional>

export module ddge.modules.ecs:ErasedComponentTable;

import ddge.utility.containers.AnyCopyable;

import :ArchetypeID;
import :component_c;
import :ComponentTable;
import :ComponentTag;
import :RecordIndex;

class ErasedComponentTable;

struct ErasedComponentTableOperations {
    using RemoveComponentFunc = auto (&)(ErasedComponentTable&, ArchetypeID, RecordIndex)
        -> void;
    using MoveComponentFunc =
        auto (&)(ErasedComponentTable&, ArchetypeID, RecordIndex, ArchetypeID)
            -> RecordIndex;
    using EmptyFunc = auto (&)(const ErasedComponentTable&) -> bool;

    RemoveComponentFunc remove_component;
    MoveComponentFunc   move_component;
    EmptyFunc           empty;
};

template <ddge::ecs::component_c Component_T>
struct ErasedComponentTableTraits {
    constexpr static auto remove_component(ErasedComponentTable&, ArchetypeID, RecordIndex)
        -> void;

    constexpr static auto
        move_component(ErasedComponentTable&, ArchetypeID, RecordIndex, ArchetypeID)
            -> RecordIndex;

    [[nodiscard]]
    constexpr static auto empty(const ErasedComponentTable&) noexcept -> bool;

    constexpr static ErasedComponentTableOperations s_operations{
        .remove_component = remove_component,
        .move_component   = move_component,
        .empty            = empty,
    };
};

class ErasedComponentTable
    : public ddge::util::
          BasicAnyCopyable<sizeof(ComponentTable<void*>), alignof(ComponentTable<void*>)> {
    using Base = ddge::util::
        BasicAnyCopyable<sizeof(ComponentTable<void*>), alignof(ComponentTable<void*>)>;

public:
    template <ddge::ecs::component_c Component_T>
    constexpr explicit ErasedComponentTable(ComponentTag<Component_T>);

    auto remove_component(ArchetypeID archetype_id, RecordIndex record_index) -> void;

    auto move_component(
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ArchetypeID new_archetype_id
    ) -> RecordIndex;

    [[nodiscard]]
    auto empty() const noexcept -> bool;

private:
    std::reference_wrapper<const ErasedComponentTableOperations> m_operations;
};

template <ddge::ecs::component_c Component_T>
constexpr auto ErasedComponentTableTraits<Component_T>::remove_component(
    ErasedComponentTable& self,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
) -> void
{
    ddge::util::any_cast<ComponentTable<Component_T>>(self).remove_component(
        archetype_id, record_index
    );
}

template <ddge::ecs::component_c Component_T>
constexpr auto ErasedComponentTableTraits<Component_T>::move_component(
    ErasedComponentTable& self,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index,
    const ArchetypeID     new_archetype_id
) -> RecordIndex
{
    return ddge::util::any_cast<ComponentTable<Component_T>>(self).move_component(
        archetype_id, record_index, new_archetype_id
    );
}

template <ddge::ecs::component_c Component_T>
constexpr auto ErasedComponentTableTraits<Component_T>::empty(
    const ErasedComponentTable& self
) noexcept -> bool
{
    return ddge::util::any_cast<ComponentTable<Component_T>>(self).empty();
}

template <ddge::ecs::component_c Component_T>
constexpr ErasedComponentTable::ErasedComponentTable(ComponentTag<Component_T>)
    : Base{ std::in_place_type<ComponentTable<Component_T>> },
      m_operations{ ErasedComponentTableTraits<Component_T>::s_operations }
{}

auto ErasedComponentTable::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> void
{
    m_operations.get().remove_component(*this, archetype_id, record_index);
}

auto ErasedComponentTable::move_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ArchetypeID new_archetype_id
) -> RecordIndex
{
    return m_operations.get().move_component(
        *this, archetype_id, record_index, new_archetype_id
    );
}

auto ErasedComponentTable::empty() const noexcept -> bool
{
    return m_operations.get().empty(*this);
}
