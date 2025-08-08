module;

#include <algorithm>
#include <optional>
#include <span>
#include <vector>

export module modules.ecs:LookupTable;

import utility.containers.Any;
import utility.containers.SlotMap;
import utility.meta.concepts.ranges.input_range_of;
import utility.meta.concepts.specialization_of;
import utility.ScopeGuard;
import utility.TypeList;
import utility.ValueSequence;

import :ComponentID;
import :ID;
import :RecordIndex;
import :RecordID;

class LookupTable {
public:
    constexpr auto emplace(modules::ecs::ID id) -> std::pair<RecordID, RecordIndex>;

    constexpr auto remove(RecordID record_id) -> std::pair<modules::ecs::ID, RecordIndex>;
    constexpr auto erase(RecordID record_id)
        -> std::optional<std::pair<modules::ecs::ID, RecordIndex>>;

    [[nodiscard]]
    constexpr auto get(RecordID record_id) const -> RecordIndex;

    [[nodiscard]]
    constexpr auto find(RecordID record_id) const noexcept -> std::optional<RecordIndex>;

    [[nodiscard]]
    constexpr auto contains(RecordID record_id) const noexcept -> bool;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;

    [[nodiscard]]
    constexpr auto ids() const noexcept -> std::span<const modules::ecs::ID>;

private:
    using SlotMap = util::SlotMap<
        RecordID::Underlying,
        modules::ecs::ID,
        (sizeof(RecordID::Underlying) - sizeof(RecordIndex::Underlying)) * 8>;
    static_assert(sizeof(RecordIndex::Underlying) == sizeof(SlotMap::Index));

    SlotMap m_ids;
};

constexpr auto LookupTable::emplace(const modules::ecs::ID id)
    -> std::pair<RecordID, RecordIndex>
{
    const auto [record_id, record_index] = m_ids.emplace(id);
    return std::make_pair(RecordID{ record_id }, RecordIndex{ record_index });
}

constexpr auto LookupTable::remove(RecordID record_id)
    -> std::pair<modules::ecs::ID, RecordIndex>
{
    const auto [id, record_index] = m_ids.remove(record_id.underlying());
    return std::make_pair(id, RecordIndex{ record_index });
}

constexpr auto LookupTable::erase(const RecordID record_id)
    -> std::optional<std::pair<modules::ecs::ID, RecordIndex>>
{
    return m_ids.erase(record_id.underlying())
        .transform([](const auto id_and_record_index) static {
            return std::tuple{ std::get<0>(id_and_record_index),
                               RecordIndex{ std::get<1>(id_and_record_index) } };
        });
}

constexpr auto LookupTable::get(const RecordID record_id) const -> RecordIndex
{
    return RecordIndex{ m_ids.get_index(record_id.underlying()) };
}

constexpr auto LookupTable::find(const RecordID record_id) const noexcept
    -> std::optional<RecordIndex>
{
    return m_ids.find_index(record_id.underlying()).transform([](const auto record_index) {
        return RecordIndex{ record_index };
    });
}

constexpr auto LookupTable::contains(const RecordID record_id) const noexcept -> bool
{
    return m_ids.contains(record_id.underlying());
}

constexpr auto LookupTable::empty() const noexcept -> bool
{
    return m_ids.empty();
}

constexpr auto LookupTable::ids() const noexcept -> std::span<const modules::ecs::ID>
{
    return m_ids.values();
}
