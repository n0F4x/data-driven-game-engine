module;

#include <cassert>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

export module utility.containers.sparse_set.SparseSet;

import utility.containers.sparse_set.key_c;
import utility.containers.sparse_set.SparseSetBase;
import utility.containers.sparse_set.value_c;

import utility.meta.type_traits.forward_like;
import utility.containers.OptionalRef;
import utility.ScopeGuard;

namespace util::sparse_set {

export template <key_c Key_T, value_c T, uint8_t version_bits_T = sizeof(Key_T) * 2>
    requires(sizeof(Key_T) * 8 > version_bits_T)
class SparseSet : SparseSetBase<Key_T, version_bits_T> {
public:
    using Key = Key_T;

    template <typename... Args>
    constexpr auto emplace(Args&&... args) -> Key;

    constexpr auto erase(Key key) -> std::optional<T>;

    template <typename Self>
    [[nodiscard]]
    constexpr auto get(this Self&&, Key key) -> meta::forward_like_t<T, Self>;

    [[nodiscard]]
    constexpr auto find(Key key) -> OptionalRef<T>;
    [[nodiscard]]
    constexpr auto find(Key key) const -> OptionalRef<const T>;

private:
    using Base = SparseSetBase<Key_T, version_bits_T>;

    using Index   = typename Base::Index;
    using Pointer = typename Base::Pointer;
    using Version = typename Base::Version;

    using Base::invalid_index;

    using Base::id_from_key;
    using Base::make_key;
    using Base::version_from_key;

    using Base::index_from_pointer;
    using Base::make_pointer;
    using Base::version_from_pointer;

    std::vector<Pointer> m_pointers;
    std::vector<T>       m_values;
    std::vector<Index>   m_ids;
    Index                oldest_dead_id{ invalid_index };
    Index                youngest_dead_id{ invalid_index };
};

}   // namespace util::sparse_set

template <util::sparse_set::key_c Key_T, util::sparse_set::value_c T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
template <typename... Args>
constexpr auto util::sparse_set::SparseSet<Key_T, T, version_bits_T>::emplace(
    Args&&... args
) -> Key
{
    m_values.emplace_back(std::forward<Args>(args)...);
    ScopeGuard value_guard{ util::make_scope_guard([this] noexcept {
        m_values.pop_back();
    }) };
    const Index index{ static_cast<Index>(m_values.size() - 1) };

    m_ids.emplace_back(index);
    ScopeGuard id_guard{ util::make_scope_guard([this] noexcept { m_ids.pop_back(); }) };

    if (oldest_dead_id == invalid_index) {
        const Index   new_id{ static_cast<Index>(m_pointers.size()) };
        const Version new_version{};
        m_pointers.push_back(make_pointer(index, new_version));
        return make_key(new_id, new_version);
    }

    const Pointer old_pointer{ m_pointers[oldest_dead_id] };
    const Index   new_id{ oldest_dead_id };
    const Version new_version{ version_from_pointer(old_pointer) };
    oldest_dead_id = index_from_pointer(old_pointer);
    if (youngest_dead_id == new_id) {
        youngest_dead_id = invalid_index;
    }
    m_pointers[new_id] = make_pointer(index, new_version);
    return make_key(new_id, new_version);
}

template <util::sparse_set::key_c Key_T, util::sparse_set::value_c T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::sparse_set::SparseSet<Key_T, T, version_bits_T>::erase(const Key key)
    -> std::optional<T>
{
    const Index id{ id_from_key(key) };
    if (id >= m_pointers.size()) {
        return std::nullopt;
    }

    const Pointer pointer{ m_pointers[id] };
    const Index   index{ index_from_pointer(pointer) };
    const Version version{ version_from_pointer(pointer) };
    if (index == invalid_index || version_from_key(key) != version) {
        return std::nullopt;
    }

    T value{ std::move(m_values[index]) };

    m_values[index] = std::move(m_values.back());
    m_values.pop_back();
    const Index moved_id = m_ids[index] = std::move(m_ids.back());
    m_ids.pop_back();
    m_pointers[moved_id] = pointer;

    if (oldest_dead_id == invalid_index) {
        oldest_dead_id = id;
    }
    if (youngest_dead_id != invalid_index) {
        m_pointers[youngest_dead_id] =
            make_pointer(id, version_from_pointer(m_pointers[youngest_dead_id]));
    }
    youngest_dead_id = id;
    m_pointers[id]   = make_pointer(invalid_index, static_cast<Version>(version + 1));

    return std::move(value);
}

template <util::sparse_set::key_c Key_T, util::sparse_set::value_c T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
template <typename Self>
constexpr auto util::sparse_set::SparseSet<Key_T, T, version_bits_T>::get(
    this Self&& self,
    const Key   key
) -> meta::forward_like_t<T, Self>
{
    const Index id{ id_from_key(key) };
    const Key   pointer{ self.m_pointers[id] };

    assert(id < self.m_pointers.size());
    assert(version_from_key(key) == version_from_pointer(pointer));

    return std::forward_like<Self>(self.m_values[index_from_pointer(pointer)]);
}

template <util::sparse_set::key_c Key_T, util::sparse_set::value_c T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::sparse_set::SparseSet<Key_T, T, version_bits_T>::find(const Key key)
    -> OptionalRef<T>
{
    const Index id{ id_from_key(key) };
    if (id >= m_pointers.size()) {
        return std::nullopt;
    }

    const Pointer pointer{ m_pointers[id] };
    const Index   index{ index_from_pointer(pointer) };
    const Version version{ version_from_pointer(pointer) };
    if (version_from_key(key) != version) {
        return std::nullopt;
    }

    return OptionalRef<T>{ m_values[index] };
}

template <util::sparse_set::key_c Key_T, util::sparse_set::value_c T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto util::sparse_set::SparseSet<Key_T, T, version_bits_T>::find(
    const Key key
) const -> OptionalRef<const T>
{
    return const_cast<SparseSet&>(*this).find(key);
}

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

using Key   = uint32_t;
using Value = int;
constexpr Value value{ 8 };
constexpr Key   missing_key{ std::numeric_limits<Key>::max() };

static_assert(
    [] {
        util::sparse_set::SparseSet<Key, Value> sparse_set;
        const Key                               key{ sparse_set.emplace(value) };

        assert(sparse_set.get(key) == value);

        return true;
    }(),
    "get test failed"
);

static_assert(
    [] {
        util::sparse_set::SparseSet<Key, Value> sparse_set;
        const Key                               key{ sparse_set.emplace(value) };

        assert(*sparse_set.find(key) == value);

        return true;
    }(),
    "find contained value test failed"
);

static_assert(
    [] {
        util::sparse_set::SparseSet<Key, Value> sparse_set;

        assert(!sparse_set.find(missing_key).has_value());

        return true;
    }(),
    "find missing value test failed"
);

static_assert(
    [] {
        util::sparse_set::SparseSet<Key, Value> sparse_set;
        const Key                               key{ sparse_set.emplace(value) };

        assert(sparse_set.erase(key));
        assert(!sparse_set.find(key).has_value());

        return true;
    }(),
    "erase contained value test failed"
);

static_assert(
    [] {
        util::sparse_set::SparseSet<Key, Value> sparse_set;

        assert(!sparse_set.erase(missing_key));

        return true;
    }(),
    "erase missing value test failed"
);

static_assert(
    [] {
        util::sparse_set::SparseSet<Key, Value> sparse_set;
        const Key                               old_key{ sparse_set.emplace(value) };
        sparse_set.erase(old_key);
        sparse_set.emplace(value);

        assert(!sparse_set.find(old_key).has_value());

        return true;
    }(),
    "version test failed"
);

#endif
