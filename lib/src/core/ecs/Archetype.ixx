module;

#include <algorithm>
#include <optional>
#include <span>
#include <vector>

export module core.ecs:Archetype;

import utility.containers.Any;
import utility.containers.SlotMap;
import utility.meta.concepts.ranges.input_range_of;
import utility.meta.concepts.specialization_of;
import utility.ScopeGuard;
import utility.ValueSequence;

import :ComponentID;
import :ID;
import :RecordIndex;
import :RecordID;
import :sorted_component_id_sequence;

class Archetype {
public:
    template <ComponentID::Underlying... component_ids>
    constexpr explicit Archetype(util::ValueSequence<ComponentID::Underlying, component_ids...>);

    [[nodiscard]]
    constexpr auto component_id_set() const -> std::span<const ComponentID>;

    template <core::ecs::component_c... Components_T>
    [[nodiscard]]
    constexpr auto contains_components() const noexcept -> bool;

    template <core::ecs::specialization_of_id_c ID_T>
    constexpr auto emplace(ID_T id) -> std::pair<RecordID, RecordIndex>;

    constexpr auto erase(RecordID record_id)
        -> std::optional<std::pair<UnderlyingID, RecordIndex>>;

    [[nodiscard]]
    constexpr auto get(RecordID record_id) const -> RecordIndex;

    [[nodiscard]]
    constexpr auto find(RecordID record_id) const noexcept -> std::optional<RecordIndex>;

    [[nodiscard]]
    constexpr auto contains(RecordID record_id) const noexcept -> bool;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;

    [[nodiscard]]
    constexpr auto ids() const noexcept -> std::span<const UnderlyingID>;

private:
    using SlotMap = util::SlotMap<
        RecordID,
        UnderlyingID,
        (sizeof(RecordID::Underlying) - sizeof(RecordIndex::Underlying)) * 8>;
    static_assert(sizeof(RecordIndex::Underlying) == sizeof(SlotMap::Index));

    std::span<const ComponentID> m_sorted_component_id_set;
    SlotMap                      m_ids;
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

    return component_id_set;
}

template <ComponentID::Underlying... component_ids>
constexpr Archetype::
    Archetype(const util::ValueSequence<ComponentID::Underlying, component_ids...>)
    : m_sorted_component_id_set{ make_component_id_set<component_ids...>() }
{}

constexpr auto Archetype::component_id_set() const -> std::span<const ComponentID>
{
    return m_sorted_component_id_set;
}

template <core::ecs::component_c... Components_T>
constexpr auto Archetype::contains_components() const noexcept -> bool
{
    return std::ranges::includes(
        m_sorted_component_id_set,
        ::sorted_component_id_sequence_t<(::component_id<Components_T>.underlying())...>::
            realize(),
        {},
        {},
        [](const auto value) { return ComponentID{ value }; }
    );
}

template <core::ecs::specialization_of_id_c ID_T>
constexpr auto Archetype::emplace(const ID_T id) -> std::pair<RecordID, RecordIndex>
{
    const auto [record_id, record_index] = m_ids.emplace(id.underlying());
    return std::make_pair(RecordID{ record_id }, RecordIndex{ record_index });
}

constexpr auto Archetype::erase(const RecordID record_id)
    -> std::optional<std::pair<UnderlyingID, RecordIndex>>
{
    return m_ids.erase(record_id).transform([](const auto id_and_record_index) static {
        return std::tuple{ std::get<0>(id_and_record_index),
                           RecordIndex{ std::get<1>(id_and_record_index) } };
    });
}

constexpr auto Archetype::get(const RecordID record_id) const -> RecordIndex
{
    return RecordIndex{ m_ids.get_index(record_id) };
}

constexpr auto Archetype::find(const RecordID record_id) const noexcept
    -> std::optional<RecordIndex>
{
    return m_ids.find_index(record_id).transform([](const auto record_index) {
        return RecordIndex{ record_index };
    });
}

constexpr auto Archetype::contains(const RecordID record_id) const noexcept -> bool
{
    return m_ids.contains(record_id);
}

constexpr auto Archetype::empty() const noexcept -> bool
{
    return m_ids.empty();
}

constexpr auto Archetype::ids() const noexcept -> std::span<const UnderlyingID>
{
    return m_ids.values();
}
