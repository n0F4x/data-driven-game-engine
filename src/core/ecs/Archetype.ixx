module;

#include <optional>
#include <span>
#include <vector>

export module core.ecs:Archetype;

import utility.containers.Any;
import utility.containers.SparseSet;
import utility.meta.reflection.type_id;
import utility.meta.type_traits.type_list.type_list_sort;
import utility.Strong;
import utility.TypeList;
import utility.ValueSequence;

import :Entity;
import :Component;
import :RegistryTag;
import :specialization_of_registry_c;

using ArchetypeID = util::Strong<uint_least32_t>;

template <core::ecs::specialization_of_registry_c Registry_T>
using ArchetypeIDGenerator = util::meta::type_id_generator_t<RegistryTag<Registry_T>{}>;

template <typename ComponentIDConstant>
struct component_hash {
    constexpr static size_t value{ ComponentIDConstant::value };
};

template <ComponentID::Underlying... component_ids>   // TODO: sort an index_sequence
using sorted_component_list_t = util::meta::type_list_sort_t<
    util::TypeList<std::integral_constant<ComponentID::Underlying, component_ids>...>,
    component_hash>;

template <
    core::ecs::specialization_of_registry_c Registry_T,
    core::ecs::component_c... Components_T>
constexpr ArchetypeID archetype_id_v{ ArchetypeIDGenerator<Registry_T>::template id_v<
    sorted_component_list_t<component_id_v<Registry_T, Components_T>.underlying()...>,
    ArchetypeID::Underlying> };

class Archetype {
public:
    template <ComponentID::Underlying... component_ids>
    constexpr explicit Archetype(util::ValueSequence<component_ids...>);

    [[nodiscard]]
    constexpr auto component_set() const -> std::span<const ComponentID>;

    constexpr auto emplace() -> std::pair<Key, Index>;

    constexpr auto erase(Key key) -> std::optional<Index>;

    [[nodiscard]]
    constexpr auto get(Key key) const -> Index;

    [[nodiscard]]
    constexpr auto find(Key key) const noexcept -> std::optional<Index>;

    [[nodiscard]]
    constexpr auto contains(Key key) const noexcept -> bool;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;

private:
    using SparseSet =
        util::SparseSet<Key, (sizeof(Key::Underlying) - sizeof(Index::Underlying)) * 8>;
    static_assert(sizeof(Index::Underlying) == sizeof(SparseSet::ID));

    std::span<const ComponentID> m_component_id_set;
    SparseSet                    m_sparse_index_set;
};

template <ComponentID::Underlying... component_ids>
[[nodiscard]]
consteval auto make_component_id_set() -> std::span<const ComponentID>
{
    using SortedComponentList = sorted_component_list_t<component_ids...>;

    constexpr static std::array<ComponentID, sizeof...(component_ids)> component_id_set =
        [] {
            std::array<ComponentID, sizeof...(component_ids)> result;

            SortedComponentList::enumerate(
                [&result]<size_t index_T, typename ComponentIDConstant> {
                    result[index_T] = ComponentID{ ComponentIDConstant::value };
                }
            );

            return result;
        }();

    return component_id_set;
}

template <ComponentID::Underlying... component_ids>
constexpr Archetype::Archetype(const util::ValueSequence<component_ids...>)
    : m_component_id_set{ make_component_id_set<component_ids...>() }
{}

constexpr auto Archetype::component_set() const -> std::span<const ComponentID>
{
    return m_component_id_set;
}

constexpr auto Archetype::emplace() -> std::pair<Key, Index>
{
    const auto [key, index] = m_sparse_index_set.emplace();
    return std::make_pair(Key{ key }, Index{ index });
}

constexpr auto Archetype::erase(const Key key) -> std::optional<Index>
{
    return m_sparse_index_set.erase(key).transform([](const auto index) {
        return Index{ index };
    });
}

constexpr auto Archetype::get(const Key key) const -> Index
{
    return Index{ m_sparse_index_set.get(key) };
}

constexpr auto Archetype::find(const Key key) const noexcept -> std::optional<Index>
{
    return m_sparse_index_set.find(key).transform([](const auto index) {
        return Index{ index };
    });
}

constexpr auto Archetype::contains(const Key key) const noexcept -> bool
{
    return m_sparse_index_set.contains(key);
}

constexpr auto Archetype::empty() const noexcept -> bool
{
    return m_sparse_index_set.empty();
}
