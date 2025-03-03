module;

#include <type_traits>
#include <utility>
#include <vector>

export module core.ecs.Component;

import utility.containers.Any;
import utility.meta.concepts.nothrow_movable;
import utility.meta.reflection.hash;
import utility.Strong;

import core.ecs.Entity;

namespace core::ecs {

export template <typename T>
concept component_c = !std::is_const_v<T> && util::meta::nothrow_movable_c<T>;

struct component_id_tag {};

export using ComponentID = ::util::Strong<util::meta::TypeHash, component_id_tag>;

export template <component_c Component_T>
constexpr ComponentID component_id_v{ util::meta::hash_v<Component_T> };

export template <component_c>
struct component_tag_t {};

export template <component_c Component_T>
constexpr component_tag_t<Component_T> component_tag{};

export template <component_c Component_T>
using ComponentContainer = std::vector<Component_T>;

export class ErasedComponentContainer;

struct ErasedComponentContainerOperations {
    using EraseFunc = auto (*)(ErasedComponentContainer&, Index) -> bool;
    using EmptyFunc = auto (*)(const ErasedComponentContainer&) -> bool;

    EraseFunc erase;
    EmptyFunc empty;
};

template <component_c>
struct ErasedComponentContainerTraits {
    [[nodiscard]]
    constexpr static auto erase(ErasedComponentContainer&, Index) -> bool;

    [[nodiscard]]
    constexpr static auto empty(const ErasedComponentContainer&) -> bool;

    constexpr static ErasedComponentContainerOperations s_operations{ erase, empty };
};

export class ErasedComponentContainer : public ::util::Any {
public:
    template <component_c Component_T>
    constexpr explicit ErasedComponentContainer(component_tag_t<Component_T>);

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;

    constexpr auto erase(Index index) -> bool;

private:
    const ErasedComponentContainerOperations* m_operations;
};

}   // namespace core::ecs

template <core::ecs::component_c Component_T>
constexpr auto core::ecs::ErasedComponentContainerTraits<Component_T>::erase(
    ErasedComponentContainer& erased_component_container,
    const Index               index
) -> bool
{
    ComponentContainer<Component_T>& component_container{
        erased_component_container.get<ComponentContainer<Component_T>>()
    };

    if (component_container.size() >= index.underlying()) {
        return false;
    }

    component_container[index.underlying()] = std::move(component_container.back());
    component_container.pop_back();

    return true;
}

template <core::ecs::component_c Component_T>
constexpr auto core::ecs::ErasedComponentContainerTraits<Component_T>::empty(
    const ErasedComponentContainer& erased_component_container
) -> bool
{
    const ComponentContainer<Component_T>& component_container{
        erased_component_container.get<ComponentContainer<Component_T>>()
    };

    return component_container.empty();
}

template <core::ecs::component_c Component_T>
constexpr core::ecs::ErasedComponentContainer::
    ErasedComponentContainer(component_tag_t<Component_T>)
    : util::Any{ std::in_place_type<std::vector<Component_T>> },
      m_operations{ &ErasedComponentContainerTraits<Component_T>::s_operations }
{}

constexpr auto core::ecs::ErasedComponentContainer::empty() const noexcept -> bool
{
    return m_operations->empty(*this);
}

constexpr auto core::ecs::ErasedComponentContainer::erase(const Index index) -> bool
{
    return m_operations->erase(*this, index);
}
