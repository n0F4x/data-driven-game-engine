#include <cstdint>
#include <limits>
#include <optional>
#include <tuple>
#include <utility>

import ddge.utility.containers.SlotMultiMap;

namespace {

using Key = uint32_t;

struct Dummy {
    double value{};

    auto operator<=>(const Dummy&) const = default;
};

using Values = std::tuple<int, float, Dummy>;

constexpr Key                           missing_key{ std::numeric_limits<Key>::max() };
constexpr std::tuple<int, float, Dummy> values{ 32, 0.7f, Dummy{ 9 } };

}   // namespace

template <>
class util::SlotMultiMap<Key, std::tuple<>>;

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                            result = multi_sparse_set.get(key);

        static_assert(std::is_same_v<decltype(result), std::tuple<int&, float&, Dummy&>>);

        return result == values;
    }(),
    "get & test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                            result = std::as_const(multi_sparse_set).get(key);

        static_assert(std::is_same_v<
                      decltype(result),
                      std::tuple<const int&, const float&, const Dummy&>>);

        return result == values;
    }(),
    "get const& test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto                            result = std::move(multi_sparse_set).get(key);

        static_assert(
            std::is_same_v<decltype(result), std::tuple<int&&, float&&, Dummy&&>>
        );

        return result == values;
    }(),
    "get && test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        auto result = std::move(std::as_const(multi_sparse_set)).get(key);

        static_assert(std::is_same_v<
                      decltype(result),
                      std::tuple<const int&&, const float&&, const Dummy&&>>);

        return result == values;
    }(),
    "get const&& test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key = multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        );
        auto result = multi_sparse_set.find(key);

        static_assert(
            std::is_same_v<decltype(result), std::optional<std::tuple<int&, float&, Dummy&>>>
        );

        return result.value() == values;
    }(),
    "find contained value test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key = multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        );
        auto result = std::as_const(multi_sparse_set).find(key);

        static_assert(std::is_same_v<
                      decltype(result),
                      std::optional<std::tuple<const int&, const float&, const Dummy&>>>);

        return result.value() == values;
    }(),
    "find const contained value test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;

        return !multi_sparse_set.find(missing_key).has_value();
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::SlotMultiMap<Key, Values> multi_sparse_set;
        const Key                       key{ multi_sparse_set.emplace(
            std::get<0>(values), std::get<1>(values), std::get<2>(values)
        ) };
        const std::optional<Values>     erased_values = multi_sparse_set.erase(key);

        return erased_values.value() == values && !multi_sparse_set.find(key).has_value();
    }(),
    "erase contained value test failed"
);
