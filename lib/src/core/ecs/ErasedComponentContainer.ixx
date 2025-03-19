module;

#include <utility>

export module core.ecs:ErasedComponentContainer;

import utility.containers.Any;
import utility.meta.concepts.nothrow_movable;

import :RecordIndex;
import :component_c;
import :ComponentContainer;
export import :ComponentTag;

class ErasedComponentContainer;

struct ErasedComponentContainerOperations {
    using EraseFunc = auto (*)(ErasedComponentContainer&, RecordIndex) -> bool;
    using EmptyFunc = auto (*)(const ErasedComponentContainer&) -> bool;
    using SizeFunc  = auto (*)(const ErasedComponentContainer&) -> size_t;

    EraseFunc erase;
    EmptyFunc empty;
    SizeFunc  size;
};

template <core::ecs::component_c>
struct ErasedComponentContainerTraits {
    [[nodiscard]]
    constexpr static auto erase(ErasedComponentContainer&, RecordIndex) -> bool;

    [[nodiscard]]
    constexpr static auto empty(const ErasedComponentContainer&) -> bool;
    [[nodiscard]]
    constexpr static auto size(const ErasedComponentContainer&) -> size_t;

    constexpr static ErasedComponentContainerOperations s_operations{ erase, empty, size };
};

class ErasedComponentContainer
    : public ::util::
          BasicAny<sizeof(ComponentContainer<void*>), alignof(ComponentContainer<void*>)> {
    using Base = util::
        BasicAny<sizeof(ComponentContainer<void*>), alignof(ComponentContainer<void*>)>;

public:
    template <core::ecs::component_c Component_T>
    explicit ErasedComponentContainer(ComponentTag<Component_T>);

    auto erase(RecordIndex record_index) -> bool;

    [[nodiscard]]
    auto empty() const noexcept -> bool;
    [[nodiscard]]
    auto size() const noexcept -> size_t;

private:
    const ErasedComponentContainerOperations* m_operations;
};

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::erase(
    ErasedComponentContainer& erased_component_container,
    const RecordIndex         record_index
) -> bool
{
    ComponentContainer<Component_T>& component_container{
        erased_component_container.get<ComponentContainer<Component_T>>()
    };

    if (record_index.underlying() >= component_container.size()) {
        return false;
    }

    component_container[record_index.underlying()] = std::move(component_container.back());
    component_container.pop_back();

    return true;
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::empty(
    const ErasedComponentContainer& erased_component_container
) -> bool
{
    const ComponentContainer<Component_T>& component_container{
        erased_component_container.get<ComponentContainer<Component_T>>()
    };

    return component_container.empty();
}

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::size(
    const ErasedComponentContainer& erased_component_container
) -> size_t
{
    const ComponentContainer<Component_T>& component_container{
        erased_component_container.get<ComponentContainer<Component_T>>()
    };

    return component_container.size();
}

template <core::ecs::component_c Component_T>
ErasedComponentContainer::ErasedComponentContainer(ComponentTag<Component_T>)
    : Base{ std::in_place_type<ComponentContainer<Component_T>> },
      m_operations{ &ErasedComponentContainerTraits<Component_T>::s_operations }
{}

auto ErasedComponentContainer::erase(const RecordIndex record_index) -> bool
{
    return m_operations->erase(*this, record_index);
}

auto ErasedComponentContainer::empty() const noexcept -> bool
{
    return m_operations->empty(*this);
}

auto ErasedComponentContainer::size() const noexcept -> size_t
{
    return m_operations->size(*this);
}
