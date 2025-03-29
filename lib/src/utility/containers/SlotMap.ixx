module;

#include <cassert>
#include <functional>
#include <optional>
#include <span>
#include <utility>
#include <vector>

#include "utility/lifetime_bound.hpp"

export module utility.containers.SlotMap;

import utility.containers.SparseSet;

import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.containers.OptionalRef;
import utility.ScopeGuard;
import utility.Strong;

// TODO: remove this namespace
namespace {
template <typename T>
concept key_c = requires { util::SparseSet<T>{}; };
}   // namespace

namespace util {

export template <
    ::key_c                         Key_T,
    ::util::meta::nothrow_movable_c T,
    uint8_t                         version_bit_size_T = sizeof(Key_T) * 2>
class SlotMap {
    using SparseSet = SparseSet<Key_T, version_bit_size_T>;

public:
    using Key   = Key_T;
    using Index = typename SparseSet::ID;
    using Value = T;

    [[nodiscard]]
    constexpr auto next_key() const noexcept -> Key;

    template <typename... Args_T>
    constexpr auto emplace(Args_T&&... args) -> std::pair<Key, Index>;
    constexpr auto erase(Key key) -> std::optional<std::pair<Value, Index>>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&, Key key) -> meta::forward_like_t<Value, Self_T>;
    [[nodiscard]]
    constexpr auto find(Key key) noexcept -> OptionalRef<Value>;
    [[nodiscard]]
    constexpr auto find(Key key) const noexcept -> OptionalRef<const Value>;
    [[nodiscard]]
    constexpr auto contains(Key key) const noexcept -> bool;
    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;
    [[nodiscard]]
    constexpr auto values() const noexcept [[lifetime_bound]] -> std::span<const Value>;

    [[nodiscard]]
    constexpr auto get_index(Key key) const -> Index;
    [[nodiscard]]
    constexpr auto find_index(Key key) const noexcept -> std::optional<Index>;

private:
    SparseSet      m_sparse_set;
    std::vector<T> m_values;
};

}   // namespace util

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::next_key() const noexcept
    -> Key
{
    return m_sparse_set.next_key();
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
template <typename... Args_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::emplace(Args_T&&... args)
    -> std::pair<Key, Index>
{
    m_values.emplace_back(std::forward<Args_T>(args)...);
    ScopeGuard _{ [this] noexcept { m_values.pop_back(); } };

    return m_sparse_set.emplace();
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::erase(const Key key)
    -> std::optional<std::pair<Value, Index>>
{
    return m_sparse_set.erase(key).transform([this](const auto index) {
        std::tuple result{ std::move(m_values[index]), index };

        m_values[index] = std::move(m_values.back());
        m_values.pop_back();

        return result;
    });
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
template <typename Self>
constexpr auto
    util::SlotMap<Key_T, T, version_bit_size_T>::get(this Self&& self, const Key key)
        -> meta::forward_like_t<Value, Self>
{
    const auto index{ self.m_sparse_set.get(key) };

    return std::forward_like<Self>(self.m_values[index]);
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::find(const Key key) noexcept
    -> OptionalRef<Value>
{
    return m_sparse_set.find(key).transform([this](const auto index) {
        return std::ref(m_values[index]);
    });
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::find(
    const Key key
) const noexcept -> OptionalRef<const Value>
{
    return const_cast<SlotMap&>(*this).find(key);
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::contains(
    const Key key
) const noexcept -> bool
{
    return m_sparse_set.contains(key);
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::empty() const noexcept -> bool
{
    assert(m_sparse_set.empty() == m_values.empty());
    return m_sparse_set.empty();
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::values() const noexcept
    -> std::span<const Value>
{
    return m_values;
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::get_index(const Key key) const
    -> Index
{
    return m_sparse_set.get(key);
}

template <::key_c Key_T, ::util::meta::nothrow_movable_c T, uint8_t version_bit_size_T>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::find_index(
    const Key key
) const noexcept -> std::optional<Index>
{
    return m_sparse_set.find(key);
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

using Key   = uint32_t;
using Value = int;
constexpr Value value{ 8 };
constexpr Key   missing_key{ std::numeric_limits<Key>::max() };

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;

        const auto key{ slot_map.next_key() };
        const auto actual_key{ slot_map.emplace().first };

        assert(key == actual_key);

        return true;
    }(),
    "emplace test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const auto                key{ slot_map.emplace().first };

        assert(slot_map.contains(key));

        return true;
    }(),
    "contains test failed"
);

static_assert(
    [] {
        const util::SlotMap<Key, Value> slot_map;

        assert(!slot_map.contains(missing_key));

        return true;
    }(),
    "contains missing test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 key{ slot_map.emplace(value).first };

        assert(slot_map.get(key) == value);

        return true;
    }(),
    "get test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 key{ slot_map.emplace(value).first };

        assert(*slot_map.find(key) == value);

        return true;
    }(),
    "find contained value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;

        assert(!slot_map.find(missing_key).has_value());

        return true;
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;

        assert(slot_map.empty());

        const Key key{ slot_map.emplace(value).first };

        assert(!slot_map.empty());
        assert(slot_map.erase(key));
        assert(!slot_map.find(key).has_value());
        assert(slot_map.empty());

        return true;
    }(),
    "erase contained value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;

        assert(!slot_map.erase(missing_key));

        return true;
    }(),
    "erase missing value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 old_key{ slot_map.emplace(value).first };
        slot_map.erase(old_key);
        slot_map.emplace(value);

        assert(!slot_map.find(old_key).has_value());

        return true;
    }(),
    "version test failed"
);

#endif
