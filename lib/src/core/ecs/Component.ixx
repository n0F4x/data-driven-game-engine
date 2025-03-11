module;

#include <type_traits>
#include <utility>
#include <vector>

export module core.ecs:Component;

import utility.containers.Any;
import utility.meta.concepts.nothrow_movable;
import utility.meta.reflection.type_id;
import utility.Strong;

import :Entity;
import :specialization_of_registry_c;
import :RegistryTag;

namespace core::ecs {

export template <typename T>
concept component_c = !std::is_const_v<T> && util::meta::nothrow_movable_c<T>;

}   // namespace core::ecs

using ComponentID = ::util::Strong<uint_least32_t>;

template <core::ecs::component_c Component_T>
constexpr ComponentID component_id_v{ util::meta::id_v<Component_T> };

template <core::ecs::component_c>
struct component_tag_t {};

template <core::ecs::component_c Component_T>
constexpr component_tag_t<Component_T> component_tag{};

template <core::ecs::component_c Component_T>
using ComponentContainer = std::vector<Component_T>;

template <core::ecs::specialization_of_registry_c Registry_T>
class ErasedComponentContainer;

template <core::ecs::specialization_of_registry_c Registry_T>
struct ErasedComponentContainerOperations {
    using EraseFunc = auto (*)(ErasedComponentContainer<Registry_T>&, Index) -> bool;
    using EmptyFunc = auto (*)(const ErasedComponentContainer<Registry_T>&) -> bool;

    EraseFunc erase;
    EmptyFunc empty;
};

template <core::ecs::specialization_of_registry_c Registry_T, core::ecs::component_c>
struct ErasedComponentContainerTraits {
    [[nodiscard]]
    constexpr static auto erase(ErasedComponentContainer<Registry_T>&, Index) -> bool;

    [[nodiscard]]
    constexpr static auto empty(const ErasedComponentContainer<Registry_T>&) -> bool;

    constexpr static ErasedComponentContainerOperations s_operations{ erase, empty };
};

template <core::ecs::specialization_of_registry_c Registry_T>
class ErasedComponentContainer : public ::util::Any {
public:
    template <core::ecs::component_c Component_T>
    constexpr explicit ErasedComponentContainer(component_tag_t<Component_T>);

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;

    constexpr auto erase(Index index) -> bool;

private:
    const ErasedComponentContainerOperations<Registry_T>* m_operations;
};

template <core::ecs::specialization_of_registry_c Registry_T, core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Registry_T, Component_T>::erase(
    ErasedComponentContainer<Registry_T>& erased_component_container,
    const Index                           index
) -> bool
{
    ComponentContainer<Component_T>& component_container{
        erased_component_container.template get<ComponentContainer<Component_T>>()
    };

    if (component_container.size() >= index.underlying()) {
        return false;
    }

    component_container[index.underlying()] = std::move(component_container.back());
    component_container.pop_back();

    return true;
}

template <core::ecs::specialization_of_registry_c Registry_T, core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Registry_T, Component_T>::empty(
    const ErasedComponentContainer<Registry_T>& erased_component_container
) -> bool
{
    const ComponentContainer<Component_T>& component_container{
        erased_component_container.template get<ComponentContainer<Component_T>>()
    };

    return component_container.empty();
}

template <core::ecs::specialization_of_registry_c Registry_T>
template <core::ecs::component_c Component_T>
constexpr ErasedComponentContainer<Registry_T>::
    ErasedComponentContainer(component_tag_t<Component_T>)
    : util::Any{ std::in_place_type<std::vector<Component_T>> },
      m_operations{ &ErasedComponentContainerTraits<Registry_T, Component_T>::s_operations }
{}

template <core::ecs::specialization_of_registry_c Registry_T>
constexpr auto ErasedComponentContainer<Registry_T>::empty() const noexcept -> bool
{
    return m_operations->empty(*this);
}

template <core::ecs::specialization_of_registry_c Registry_T>
constexpr auto ErasedComponentContainer<Registry_T>::erase(const Index index) -> bool
{
    return m_operations->erase(*this, index);
}
