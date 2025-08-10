module;

#include <unordered_map>

#include "utility/contracts_macros.hpp"

export module ddge.modules.ecs:LookupTableMap;

import ddge.utility.contracts;

import :ArchetypeID;
import :ID;
import :LookupTable;
import :RecordID;
import :RecordIndex;

class LookupTableMap {
    using Container = std::unordered_map<ArchetypeID, LookupTable>;

public:
    using Iterator = Container::iterator;

    [[nodiscard]]
    auto insert(ddge::ecs::ID id, ArchetypeID archetype_id)
        -> std::pair<RecordID, RecordIndex>;

    auto remove(ArchetypeID archetype_id, RecordID record_id)
        -> std::pair<ddge::ecs::ID, RecordIndex>;
    auto remove(Iterator iterator, RecordID record_id)
        -> std::pair<ddge::ecs::ID, RecordIndex>;

    [[nodiscard]]
    auto get_lookup_table(ArchetypeID archetype_id) -> LookupTable&;
    [[nodiscard]]
    auto get_lookup_table(ArchetypeID archetype_id) const -> const LookupTable&;

    [[nodiscard]]
    auto get_iterator(ArchetypeID archetype_id) -> Container::iterator;
    [[nodiscard]]
    auto get_iterator(ArchetypeID archetype_id) const -> Container::const_iterator;

    [[nodiscard]]
    auto get_record_index(ArchetypeID archetype_id, RecordID record_id) const
        -> RecordIndex;

    [[nodiscard]]
    auto contains(ArchetypeID archetype_id) const -> bool;

private:
    Container m_map;
};

auto LookupTableMap::insert(const ddge::ecs::ID id, const ArchetypeID archetype_id)
    -> std::pair<RecordID, RecordIndex>
{
    return m_map[archetype_id].emplace(id);
}

auto LookupTableMap::remove(const ArchetypeID archetype_id, const RecordID record_id)
    -> std::pair<ddge::ecs::ID, RecordIndex>
{
    const auto iterator{ m_map.find(archetype_id) };
    PRECOND(iterator != m_map.cend());

    return remove(iterator, record_id);
}

auto LookupTableMap::remove(const Iterator iterator, const RecordID record_id)
    -> std::pair<ddge::ecs::ID, RecordIndex>
{
    std::pair<ddge::ecs::ID, RecordIndex> result = iterator->second.remove(record_id);

    if (iterator->second.empty()) {
        m_map.erase(iterator);
    }

    return result;
}

auto LookupTableMap::get_lookup_table(const ArchetypeID archetype_id) -> LookupTable&
{
    const auto iterator{ m_map.find(archetype_id) };
    PRECOND(iterator != m_map.cend());

    return iterator->second;
}

auto LookupTableMap::get_lookup_table(const ArchetypeID archetype_id) const
    -> const LookupTable&
{
    return const_cast<LookupTableMap&>(*this).get_lookup_table(archetype_id);
}

auto LookupTableMap::get_iterator(const ArchetypeID archetype_id) -> Iterator
{
    const Iterator result = m_map.find(archetype_id);
    PRECOND(result != m_map.cend());
    return result;
}

auto LookupTableMap::get_iterator(const ArchetypeID archetype_id) const
    -> Container::const_iterator
{
    return const_cast<LookupTableMap&>(*this).get_iterator(archetype_id);
}

auto LookupTableMap::get_record_index(
    const ArchetypeID archetype_id,
    const RecordID    record_id
) const -> RecordIndex
{
    return get_lookup_table(archetype_id).get(record_id);
}

auto LookupTableMap::contains(const ArchetypeID archetype_id) const -> bool
{
    return m_map.contains(archetype_id);
}
