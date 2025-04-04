module;

#include <optional>
#include <utility>

#include <gsl/gsl-lite.hpp>

#include "utility/contracts.hpp"

export module core.ecs:ErasedComponentContainer;

import utility.containers.Any;
import utility.meta.concepts.nothrow_movable;

import :ArchetypeID;
import :component_c;
import :ComponentContainer;
import :ComponentTable;
import :ComponentTag;
import :RecordIndex;

class ErasedComponentContainer;

struct ErasedComponentContainerOperations {
    using EraseFunc  = auto (*)(ErasedComponentContainer&, RecordIndex) -> bool;
    using RemoveFunc = auto (*)(ErasedComponentContainer&, RecordIndex) -> void;
    using EmptyFunc  = auto (*)(const ErasedComponentContainer&) -> bool;
    using SizeFunc   = auto (*)(const ErasedComponentContainer&) -> size_t;
    using MoveOutToFunc =
        auto (*)(ErasedComponentContainer&, RecordIndex, ComponentTable&, ArchetypeID)
            -> RecordIndex;

    gsl::not_null_ic<EraseFunc>     erase;
    gsl::not_null_ic<RemoveFunc>    remove;
    gsl::not_null_ic<EmptyFunc>     empty;
    gsl::not_null_ic<SizeFunc>      size;
    gsl::not_null_ic<MoveOutToFunc> move_out_to;
};

template <core::ecs::component_c Component_T>
struct ErasedComponentContainerTraits {
    constexpr static auto erase(ErasedComponentContainer&, RecordIndex) -> bool;
    constexpr static auto remove(ErasedComponentContainer&, RecordIndex) -> void;

    [[nodiscard]]
    constexpr static auto empty(const ErasedComponentContainer&) -> bool;
    [[nodiscard]]
    constexpr static auto size(const ErasedComponentContainer&) -> size_t;

    constexpr static auto
        move_out_to(ErasedComponentContainer&, RecordIndex, ComponentTable&, ArchetypeID)
            -> RecordIndex;

    constexpr static ErasedComponentContainerOperations s_operations{
        .erase       = erase,
        .remove      = remove,
        .empty       = empty,
        .size        = size,
        .move_out_to = move_out_to,
    };

private:
    constexpr static auto erase_impl(ErasedComponentContainer&, RecordIndex)
        -> std::optional<Component_T>;
    constexpr static auto remove_impl(ErasedComponentContainer&, RecordIndex)
        -> Component_T;
};

class ErasedComponentContainer
    : public ::util::
          BasicAny<sizeof(ComponentContainer<void*>), alignof(ComponentContainer<void*>)> {
    using Base = util::
        BasicAny<sizeof(ComponentContainer<void*>), alignof(ComponentContainer<void*>)>;

public:
    template <core::ecs::component_c Component_T>
    constexpr explicit ErasedComponentContainer(ComponentTag<Component_T>);

    constexpr auto erase(RecordIndex record_index) -> bool;
    constexpr auto remove(RecordIndex record_index) -> void;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;
    [[nodiscard]]
    constexpr auto size() const noexcept -> size_t;

    auto move_out_to(
        RecordIndex     record_index,
        ComponentTable& component_table,
        ArchetypeID     archetype_id
    ) -> RecordIndex;

private:
    const ErasedComponentContainerOperations* m_operations;
};

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::erase(
    ErasedComponentContainer& self,
    const RecordIndex         record_index
) -> bool
{
    return erase_impl(self, record_index).has_value();
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::remove(
    ErasedComponentContainer& self,
    const RecordIndex         record_index
) -> void
{
    remove_impl(self, record_index);
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::empty(
    const ErasedComponentContainer& self
) -> bool
{
    const ComponentContainer<Component_T>& component_container{
        self.get<ComponentContainer<Component_T>>()
    };

    return component_container.empty();
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::size(
    const ErasedComponentContainer& self
) -> size_t
{
    const ComponentContainer<Component_T>& component_container{
        self.get<ComponentContainer<Component_T>>()
    };

    return component_container.size();
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::move_out_to(
    ErasedComponentContainer& self,
    const RecordIndex         record_index,
    ComponentTable&           component_table,
    const ArchetypeID         archetype_id
) -> RecordIndex
{
    PRECOND(archetype_id->contains_all_of_components<Component_T>());

    ErasedComponentContainer& other{
        component_table.try_emplace(archetype_id, component_tag<Component_T>).first->second
    };

    other
        .get<ComponentContainer<Component_T>>()   //
        .push_back(remove_impl(self, record_index));

    return RecordIndex{ static_cast<RecordIndex::Underlying>(other.size() - 1uz) };
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::erase_impl(
    ErasedComponentContainer& self,
    const RecordIndex         record_index
) -> std::optional<Component_T>
{
    ComponentContainer<Component_T>& component_container{
        self.get<ComponentContainer<Component_T>>()
    };

    if (record_index.underlying() >= component_container.size()) {
        return std::nullopt;
    }

    std::optional<Component_T> result{
        std::move(component_container[record_index.underlying()])
    };

    component_container[record_index.underlying()] = std::move(component_container.back());
    component_container.pop_back();

    return result;
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::remove_impl(
    ErasedComponentContainer& self,
    const RecordIndex         record_index
) -> Component_T
{
    ComponentContainer<Component_T>& component_container{
        self.get<ComponentContainer<Component_T>>()
    };

    PRECOND(record_index.underlying() < component_container.size());

    Component_T result{ std::move(component_container[record_index.underlying()]) };

    component_container[record_index.underlying()] = std::move(component_container.back());
    component_container.pop_back();

    return result;
}

template <core::ecs::component_c Component_T>
constexpr ErasedComponentContainer::ErasedComponentContainer(ComponentTag<Component_T>)
    : Base{ std::in_place_type<ComponentContainer<Component_T>> },
      m_operations{ &ErasedComponentContainerTraits<Component_T>::s_operations }
{}

constexpr auto ErasedComponentContainer::erase(const RecordIndex record_index) -> bool
{
    return m_operations->erase.operator->()(*this, record_index);
}

constexpr auto ErasedComponentContainer::remove(const RecordIndex record_index) -> void
{
    return m_operations->remove.operator->()(*this, record_index);
}

constexpr auto ErasedComponentContainer::empty() const noexcept -> bool
{
    return m_operations->empty.operator->()(*this);
}

constexpr auto ErasedComponentContainer::size() const noexcept -> size_t
{
    return m_operations->size.operator->()(*this);
}

auto ErasedComponentContainer::move_out_to(
    const RecordIndex record_index,
    ComponentTable&   component_table,
    const ArchetypeID archetype_id
) -> RecordIndex
{
    return m_operations->move_out_to.operator->()(
        *this, record_index, component_table, archetype_id
    );
}
