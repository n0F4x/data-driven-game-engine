module;

#include <cassert>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

export module utility.containers.SlotMap;

import utility.containers.SparseSet;

import utility.meta.concepts.nothrow_movable;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.containers.OptionalRef;
import utility.ScopeGuard;
import utility.Strong;

namespace util {

export template <
    specialization_of_strong_c      Key_T,
    ::util::meta::nothrow_movable_c T,
    uint8_t version_bit_size_T = sizeof(typename Key_T::Underlying) * 2>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
class SlotMap {
public:
    using Key   = Key_T;
    using Value = T;

    template <typename... Args>
    constexpr auto emplace(Args&&... args) -> Key;

    constexpr auto erase(Key key) -> std::optional<Value>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&, Key key) -> meta::forward_like_t<Value, Self_T>;

    [[nodiscard]]
    constexpr auto find(Key key) noexcept -> OptionalRef<Value>;
    [[nodiscard]]
    constexpr auto find(Key key) const noexcept -> OptionalRef<const Value>;

    [[nodiscard]]
    constexpr auto contains(Key key) const noexcept -> bool;

private:
    SparseSet<Key, version_bit_size_T> m_sparse_set;
    std::vector<T>                     m_values;
};

}   // namespace util

template <
    util::specialization_of_strong_c Key_T,
    util::meta::nothrow_movable_c    T,
    uint8_t                          version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
template <typename... Args>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::emplace(Args&&... args) -> Key
{
    m_values.emplace_back(std::forward<Args>(args)...);
    ScopeGuard _{ [this] noexcept { m_values.pop_back(); } };

    return m_sparse_set.emplace().first;
}

template <
    util::specialization_of_strong_c Key_T,
    util::meta::nothrow_movable_c    T,
    uint8_t                          version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::erase(const Key key)
    -> std::optional<Value>
{
    return m_sparse_set.erase(key).transform([this](const auto index) {
        Value value{ std::move(m_values[index]) };

        m_values[index] = std::move(m_values.back());
        m_values.pop_back();

        return value;
    });
}

template <
    util::specialization_of_strong_c Key_T,
    util::meta::nothrow_movable_c    T,
    uint8_t                          version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
template <typename Self>
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::get(
    this Self&& self,
    const Key   key
) -> meta::forward_like_t<Value, Self>
{
    const auto index{ self.m_sparse_set.get(key) };

    return std::forward_like<Self>(self.m_values[index]);
}

template <
    util::specialization_of_strong_c Key_T,
    util::meta::nothrow_movable_c    T,
    uint8_t                          version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::find(const Key key) noexcept
    -> OptionalRef<Value>
{
    return m_sparse_set.find(key).transform([this](const auto index) {
        return std::ref(m_values[index]);
    });
}

template <
    util::specialization_of_strong_c Key_T,
    util::meta::nothrow_movable_c    T,
    uint8_t                          version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::find(
    const Key key
) const noexcept -> OptionalRef<const Value>
{
    return const_cast<SlotMap&>(*this).find(key);
}

template <
    util::specialization_of_strong_c Key_T,
    ::util::meta::nothrow_movable_c  T,
    uint8_t                          version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::contains(
    const Key key
) const noexcept -> bool
{
    return m_sparse_set.contains(key);
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

using Key   = util::Strong<uint32_t>;
using Value = int;
constexpr Value value{ 8 };
constexpr Key   missing_key{ std::numeric_limits<Key>::max() };

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 key{ slot_map.emplace() };

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
        const Key                 key{ slot_map.emplace(value) };

        assert(slot_map.get(key) == value);

        return true;
    }(),
    "get test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> slot_map;
        const Key                 key{ slot_map.emplace(value) };

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
        const Key                 key{ slot_map.emplace(value) };

        assert(slot_map.erase(key));
        assert(!slot_map.find(key).has_value());

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
        const Key                 old_key{ slot_map.emplace(value) };
        slot_map.erase(old_key);
        slot_map.emplace(value);

        assert(!slot_map.find(old_key).has_value());

        return true;
    }(),
    "version test failed"
);

#endif
