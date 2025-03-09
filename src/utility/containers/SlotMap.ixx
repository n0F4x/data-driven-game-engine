module;

#include <cassert>
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

namespace {   // TODO: remove this namespace when Clang allows it

template <typename>
struct is_specialization_of_strong : std::false_type {};

template <typename T, auto tag_T>
struct is_specialization_of_strong<util::Strong<T, tag_T>> : std::true_type {};

template <typename T>
concept specialization_of_strong_c = is_specialization_of_strong<T>::value;
}   // namespace

namespace util {

export template <
    ::specialization_of_strong_c    Key_T,
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

    template <typename Self>
    [[nodiscard]]
    constexpr auto get(this Self&&, Key key) -> meta::forward_like_t<Value, Self>;

    [[nodiscard]]
    constexpr auto find(Key key) -> OptionalRef<Value>;
    [[nodiscard]]
    constexpr auto find(Key key) const -> OptionalRef<const Value>;

private:
    SparseSet<Key, version_bit_size_T> m_sparse_set;
    std::vector<T>                     m_values;
};

}   // namespace util

template <
    specialization_of_strong_c    Key_T,
    util::meta::nothrow_movable_c T,
    uint8_t                       version_bit_size_T>
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
    specialization_of_strong_c    Key_T,
    util::meta::nothrow_movable_c T,
    uint8_t                       version_bit_size_T>
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
    specialization_of_strong_c    Key_T,
    util::meta::nothrow_movable_c T,
    uint8_t                       version_bit_size_T>
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
    specialization_of_strong_c    Key_T,
    util::meta::nothrow_movable_c T,
    uint8_t                       version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::find(const Key key)
    -> OptionalRef<Value>
{
    return m_sparse_set.find(key).transform([this](const auto index) {
        return std::ref(m_values[index]);
    });
}

template <
    specialization_of_strong_c    Key_T,
    util::meta::nothrow_movable_c T,
    uint8_t                       version_bit_size_T>
    requires std::unsigned_integral<typename Key_T::Underlying>
          && (!std::is_const_v<Key_T>)
constexpr auto util::SlotMap<Key_T, T, version_bit_size_T>::find(const Key key) const
    -> OptionalRef<const Value>
{
    return const_cast<SlotMap&>(*this).find(key);
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

using Key   = util::Strong<uint32_t>;
using Value = int;
constexpr Value value{ 8 };
constexpr Key   missing_key{ std::numeric_limits<Key>::max() };

static_assert(
    [] {
        util::SlotMap<Key, Value> sparse_set;
        const Key                 key{ sparse_set.emplace(value) };

        assert(sparse_set.get(key) == value);

        return true;
    }(),
    "get test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> sparse_set;
        const Key                 key{ sparse_set.emplace(value) };

        assert(*sparse_set.find(key) == value);

        return true;
    }(),
    "find contained value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> sparse_set;

        assert(!sparse_set.find(missing_key).has_value());

        return true;
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> sparse_set;
        const Key                 key{ sparse_set.emplace(value) };

        assert(sparse_set.erase(key));
        assert(!sparse_set.find(key).has_value());

        return true;
    }(),
    "erase contained value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> sparse_set;

        assert(!sparse_set.erase(missing_key));

        return true;
    }(),
    "erase missing value test failed"
);

static_assert(
    [] {
        util::SlotMap<Key, Value> sparse_set;
        const Key                 old_key{ sparse_set.emplace(value) };
        sparse_set.erase(old_key);
        sparse_set.emplace(value);

        assert(!sparse_set.find(old_key).has_value());

        return true;
    }(),
    "version test failed"
);

#endif
