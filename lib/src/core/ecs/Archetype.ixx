module;

#include <algorithm>
#include <optional>
#include <span>
#include <vector>

export module core.ecs:Archetype;

import utility.containers.Any;
import utility.containers.SparseSet;
import utility.meta.concepts.ranges.input_range_of;
import utility.meta.type_traits.integer_sequence.integer_sequence_sort;
import utility.meta.reflection.type_id;
import utility.Strong;
import utility.TypeList;
import utility.ValueSequence;

import :Entity;
import :Component;
import :RegistryTag;
import :specialization_of_registry_c;

// TODO
// don't use explicit template parameters once Clang name mangling bug is fixed
// probably this one (https://github.com/llvm/llvm-project/issues/118137)
using ArchetypeID = util::Strong<uint_least32_t, void, [] {}>;

template <ComponentID::Underlying... component_ids>
using sorted_component_id_sequence_t = util::meta::
    integer_sequence_sort_t<util::ValueSequence<ComponentID::Underlying, component_ids...>>;

template <core::ecs::component_c... Components_T>
constexpr ArchetypeID archetype_id_v{
    util::meta::
        id_v<sorted_component_id_sequence_t<component_id_v<Components_T>.underlying()...>>
};

class Archetype {
public:
    template <ComponentID::Underlying... component_ids>
    constexpr explicit Archetype(util::ValueSequence<ComponentID::Underlying, component_ids...>);

    [[nodiscard]]
    constexpr auto component_id_set() const -> std::span<const ComponentID>;

    template <core::ecs::component_c... Components_T>
    [[nodiscard]]
    constexpr auto contains_components() const noexcept -> bool;

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

    std::span<const ComponentID> m_sorted_component_id_set;
    SparseSet                    m_sparse_index_set;
};

template <ComponentID::Underlying... component_ids>
[[nodiscard]]
constexpr auto make_component_id_set() -> std::span<const ComponentID>
{
    using SortedComponentIDSequence = sorted_component_id_sequence_t<component_ids...>;

    constexpr static std::array<ComponentID, sizeof...(component_ids)> component_id_set =
        [] {
            std::array<ComponentID, sizeof...(component_ids)> result;

            SortedComponentIDSequence::enumerate(
                [&result]<size_t index_T, ComponentID::Underlying value_T> {
                    result[index_T] = ComponentID{ value_T };
                }
            );

            return result;
        }();

    static_assert(component_id_set[0].underlying() != 0);
    static_assert(component_id_set[1].underlying() != 0);

    return component_id_set;
}

template <ComponentID::Underlying... component_ids>
constexpr Archetype::
    Archetype(const util::ValueSequence<ComponentID::Underlying, component_ids...>)
    : m_sorted_component_id_set{ make_component_id_set<component_ids...>() }
{}

template <core::ecs::component_c... Components_T>
constexpr auto Archetype::contains_components() const noexcept -> bool
{
    return std::ranges::includes(
        m_sorted_component_id_set,
        ::sorted_component_id_sequence_t<(::component_id_v<Components_T>.underlying()
        )...>::realize(),
        {},
        {},
        [](const auto value) { return ComponentID{ value }; }
    );
}

constexpr auto Archetype::component_id_set() const -> std::span<const ComponentID>
{
    return m_sorted_component_id_set;
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
