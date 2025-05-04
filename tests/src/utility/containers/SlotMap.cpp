#include <cstdint>
#include <limits>

import utility.containers.SlotMap;

namespace {
using Key   = uint32_t;
using Value = int;
constexpr Value value{ 8 };
constexpr Key   missing_key{ std::numeric_limits<Key>::max() };
}   // namespace

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;

        const auto key{ slot_map.next_key() };
        const auto actual_key{ slot_map.emplace().first };

        return key == actual_key;
    }(),
    "emplace test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const auto                key{ slot_map.emplace().first };

        return slot_map.contains(key);
    }(),
    "contains test failed"
);

static_assert(
    [] {
        const util::SlotMap<Key, Value> slot_map;

        return !slot_map.contains(missing_key);
    }(),
    "contains missing test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 key{ slot_map.emplace(value).first };

        return slot_map.get(key) == value;
    }(),
    "get test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 key{ slot_map.emplace(value).first };

        return *slot_map.find(key) == value;
    }(),
    "find contained value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;

        return !slot_map.find(missing_key).has_value();
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 key{ slot_map.emplace(value).first };

        return slot_map.erase(key) && !slot_map.find(key).has_value();
    }(),
    "erase contained value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;

        return !slot_map.erase(missing_key);
    }(),
    "erase missing value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 old_key{ slot_map.emplace(value).first };
        slot_map.erase(old_key);
        slot_map.emplace(value);

        return !slot_map.find(old_key).has_value();
    }(),
    "version test failed"
);
