#include <cstdint>
#include <limits>
#include <tuple>

import ddge.utility.containers.SparseSet;

namespace {

using Key = uint32_t;

constexpr Key missing_key{ std::numeric_limits<Key>::max() };

}   // namespace

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;

        const std::tuple key_and_id{ sparse_set.next_key(), sparse_set.next_id() };
        const std::tuple actual_key_and_id{ sparse_set.emplace() };

        return key_and_id == actual_key_and_id;
    }(),
    "emplace test failed"
);

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;
        const auto [key, _id]{ sparse_set.emplace() };

        return sparse_set.contains(key);
    }(),
    "contains test failed"
);

static_assert(
    [] {
        const ddge::util::SparseSet<Key> sparse_set;

        return !sparse_set.contains(missing_key);
    }(),
    "contains missing test failed"
);

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;
        const auto [key, id]{ sparse_set.emplace() };

        return sparse_set.get(key) == id;
    }(),
    "get test failed"
);

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;
        const auto [key, id]{ sparse_set.emplace() };

        return sparse_set.find(key).value() == id;
    }(),
    "find contained test failed"
);

static_assert(
    [] {
        const ddge::util::SparseSet<Key> sparse_set;

        return !sparse_set.find(missing_key).has_value();
    }(),
    "find missing test failed"
);

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;
        const auto [key, _]{ sparse_set.emplace() };

        return sparse_set.erase(key) && !sparse_set.find(key).has_value();
    }(),
    "erase contained value test failed"
);

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;

        return !sparse_set.erase(missing_key);
    }(),
    "erase missing value test failed"
);

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;
        const auto [old_key, _]{ sparse_set.emplace() };
        sparse_set.erase(old_key);
        const auto [new_key, _]{ sparse_set.emplace() };

        return !sparse_set.find(old_key).has_value() && sparse_set.contains(new_key);
    }(),
    "version test failed"
);

static_assert(
    [] {
        ddge::util::SparseSet<Key> sparse_set;
        const auto [first_key, _]{ sparse_set.emplace() };
        const auto [second_key, _]{ sparse_set.emplace() };
        sparse_set.erase(second_key);
        const auto [third_key, _]{ sparse_set.emplace() };
        sparse_set.erase(first_key);

        return sparse_set.contains(third_key);
    }(),
    "complex erase test failed"
);
