module;

#include <cassert>
#include <optional>
#include <utility>
#include <vector>

export module utility.containers.SlotMap;

import utility.containers.SparseSet;

import utility.meta.concepts.nothrow_movable;
import utility.meta.type_traits.forward_like;
import utility.containers.OptionalRef;
import utility.ScopeGuard;

namespace util {

export template <
    typename Key_T,
    meta::nothrow_movable_c T,
    uint8_t                 version_bits_T = sizeof(Key_T) * 2>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
          && (sizeof(Key_T) * 8 > version_bits_T)
class SparseMap {
public:
    using Key   = Key_T;
    using Value = T;

    template <typename... Args>
    constexpr auto emplace(Args&&... args) -> Key;

    constexpr auto erase(Key key) -> std::optional<Value>;

    template <typename Self>
    [[nodiscard]]
    constexpr auto get(this Self&&, Key key) -> meta::forward_like_t<Value, Self>;

    [[nodiscard]]
    constexpr auto find(Key key) -> OptionalRef<Value>;
    [[nodiscard]]
    constexpr auto find(Key key) const -> OptionalRef<const Value>;

private:
    SparseSet<Key, version_bits_T> m_sparse_set;
    std::vector<T>                 m_values;
};

}   // namespace util

template <typename Key_T, util::meta::nothrow_movable_c T, uint8_t version_bits_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
          && (sizeof(Key_T) * 8 > version_bits_T)
template <typename... Args>
constexpr auto util::SparseMap<Key_T, T, version_bits_T>::emplace(Args&&... args) -> Key
{
    m_values.emplace_back(std::forward<Args>(args)...);
    ScopeGuard _{ [this] noexcept { m_values.pop_back(); } };

    return m_sparse_set.emplace().first;
}

template <typename Key_T, util::meta::nothrow_movable_c T, uint8_t version_bits_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
          && (sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::SparseMap<Key_T, T, version_bits_T>::erase(const Key key)
    -> std::optional<Value>
{
    return m_sparse_set.erase(key).transform([this](const auto index) {
        Value value{ std::move(m_values[index]) };

        m_values[index] = std::move(m_values.back());
        m_values.pop_back();

        return value;
    });
}

template <typename Key_T, util::meta::nothrow_movable_c T, uint8_t version_bits_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
          && (sizeof(Key_T) * 8 > version_bits_T)
template <typename Self>
constexpr auto util::SparseMap<Key_T, T, version_bits_T>::get(
    this Self&& self,
    const Key   key
) -> meta::forward_like_t<Value, Self>
{
    const auto index{ self.m_sparse_set.get(key) };

    return std::forward_like<Self>(self.m_values[index]);
}

template <typename Key_T, util::meta::nothrow_movable_c T, uint8_t version_bits_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
          && (sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::SparseMap<Key_T, T, version_bits_T>::find(const Key key)
    -> OptionalRef<Value>
{
    return m_sparse_set.find(key).transform([this](const auto index) {
        return std::ref(m_values[index]);
    });
}

template <typename Key_T, util::meta::nothrow_movable_c T, uint8_t version_bits_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
          && (sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::SparseMap<Key_T, T, version_bits_T>::find(const Key key) const
    -> OptionalRef<const Value>
{
    return const_cast<SparseMap&>(*this).find(key);
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

using Key   = uint32_t;
using Value = int;
constexpr Value value{ 8 };
constexpr Key   missing_key{ std::numeric_limits<Key>::max() };

static_assert(
    [] {
        util::SparseMap<Key, Value> sparse_set;
        const Key                   key{ sparse_set.emplace(value) };

        assert(sparse_set.get(key) == value);

        return true;
    }(),
    "get test failed"
);

static_assert(
    [] {
        util::SparseMap<Key, Value> sparse_set;
        const Key                   key{ sparse_set.emplace(value) };

        assert(*sparse_set.find(key) == value);

        return true;
    }(),
    "find contained value test failed"
);

static_assert(
    [] {
        util::SparseMap<Key, Value> sparse_set;

        assert(!sparse_set.find(missing_key).has_value());

        return true;
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::SparseMap<Key, Value> sparse_set;
        const Key                   key{ sparse_set.emplace(value) };

        assert(sparse_set.erase(key));
        assert(!sparse_set.find(key).has_value());

        return true;
    }(),
    "erase contained value test failed"
);

static_assert(
    [] {
        util::SparseMap<Key, Value> sparse_set;

        assert(!sparse_set.erase(missing_key));

        return true;
    }(),
    "erase missing value test failed"
);

static_assert(
    [] {
        util::SparseMap<Key, Value> sparse_set;
        const Key                   old_key{ sparse_set.emplace(value) };
        sparse_set.erase(old_key);
        sparse_set.emplace(value);

        assert(!sparse_set.find(old_key).has_value());

        return true;
    }(),
    "version test failed"
);

#endif
