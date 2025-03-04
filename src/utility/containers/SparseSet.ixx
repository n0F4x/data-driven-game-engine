module;

#include <bitset>
#include <cassert>
#include <concepts>
#include <limits>
#include <ranges>
#include <type_traits>
#include <vector>

export module utility.containers.SparseSet;

import utility.meta.uint_at_least;
import utility.ScopeGuard;

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
          && (sizeof(Key_T) * 8 > version_bit_size_T)
class SparseSetTraits {
public:
    using Key = Key_T;
    constexpr static uint8_t key_bit_size{ sizeof(Key) * 8 };

    constexpr static uint8_t version_bit_size{ version_bit_size_T };
    constexpr static uint8_t first_version_bit{};
    using Version = util::meta::uint_at_least_t<version_bit_size>;
    constexpr static Key version_mask{ [] {
        std::bitset<key_bit_size> mask{};
        for (const auto i : std::views::iota(
                 first_version_bit,
                 static_cast<uint8_t>(first_version_bit + version_bit_size)
             ))
        {
            mask.set(key_bit_size - i - 1);
        }
        return static_cast<Key>(mask.to_ullong());
    }() };

    constexpr static uint8_t index_bit_size{ key_bit_size - version_bit_size };
    constexpr static uint8_t first_index_bit{ version_bit_size_T };
    using Index = util::meta::uint_at_least_t<index_bit_size>;
    constexpr static Key index_mask{ [] {
        std::bitset<key_bit_size> mask{};
        for (const auto i : std::views::iota(
                 first_index_bit, static_cast<uint8_t>(first_index_bit + index_bit_size)
             ))
        {
            mask.set(key_bit_size - i - 1);
        }
        return static_cast<Key>(mask.to_ullong());
    }() };
    constexpr static Index invalid_index{ (std::numeric_limits<Key>::max() & index_mask)
                                          >> (key_bit_size - first_index_bit
                                              - index_bit_size) };

    using Pointer = Key;

    using ID = Index;
    constexpr static ID invalid_id{ invalid_index };

    [[nodiscard]]
    constexpr static auto index_from_key(const Key key) noexcept -> Index
    {
        return static_cast<Index>(
            (key & index_mask) >> (key_bit_size - first_index_bit - index_bit_size)
        );
    }

    [[nodiscard]]
    constexpr static auto version_from_key(const Key key) noexcept -> Version
    {
        return static_cast<Version>(
            (key & version_mask) >> (key_bit_size - first_version_bit - version_bit_size)
        );
    }

    [[nodiscard]]
    constexpr static auto make_key(const Index index, const Version version) noexcept
        -> Key
    {
        return ((Key{ index } << (key_bit_size - first_index_bit - index_bit_size))
                & index_mask)
             + ((Key{ version } << (key_bit_size - first_version_bit - version_bit_size))
                & version_mask);
    }

    [[nodiscard]]
    constexpr static auto id_from_pointer(const Pointer pointer) noexcept -> ID
    {
        static_assert(std::is_same_v<Pointer, Key>);
        static_assert(std::is_same_v<ID, Index>);
        return index_from_key(pointer);
    }

    [[nodiscard]]
    constexpr static auto version_from_pointer(const Pointer pointer) noexcept -> Version
    {
        static_assert(std::is_same_v<Pointer, Key>);
        return version_from_key(pointer);
    }

    [[nodiscard]]
    constexpr static auto make_pointer(const ID id, const Version version) noexcept
        -> Pointer
    {
        static_assert(std::is_same_v<Pointer, Key>);
        static_assert(std::is_same_v<ID, Index>);
        return make_key(id, version);
    }
};

template <typename Key_T, uint8_t version_bit_size_T = sizeof(Key_T) * 2>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
class SparseSet : SparseSetTraits<Key_T, version_bit_size_T> {
    using Base = SparseSetTraits<Key_T, version_bit_size_T>;

public:
    using Key = typename Base::Key;
    using ID  = typename Base::ID;

    constexpr static uint8_t version_bit_size{ Base::version_bit_size };

    constexpr auto emplace() -> std::pair<Key, ID>;

    constexpr auto erase(Key key) -> std::optional<ID>;

    [[nodiscard]]
    constexpr auto get(Key key) const -> ID;

    [[nodiscard]]
    constexpr auto find(Key key) const noexcept -> std::optional<ID>;

    [[nodiscard]]
    constexpr auto contains(Key key) const noexcept -> bool;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;

private:
    using Index   = typename Base::Index;
    using Pointer = typename Base::Pointer;
    using Version = typename Base::Version;

    using Base::invalid_id;
    using Base::invalid_index;

    using Base::index_from_key;
    using Base::make_key;
    using Base::version_from_key;

    using Base::id_from_pointer;
    using Base::make_pointer;
    using Base::version_from_pointer;

    std::vector<Pointer> m_pointers;
    std::vector<Index>   m_indices;
    Index                m_oldest_dead_index{ invalid_index };
    Index                m_youngest_dead_index{ invalid_index };

    [[nodiscard]]
    constexpr auto emplace_pointer(ID id);
};

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
constexpr auto SparseSet<Key_T, version_bit_size_T>::emplace() -> std::pair<Key, ID>
{
    const ID id{ static_cast<ID>(m_indices.size()) };

    const auto [index, version, scope_guard] = emplace_pointer(id);

    m_indices.push_back(index);

    return std::make_pair(make_key(index, version), id);
}

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
constexpr auto SparseSet<Key_T, version_bit_size_T>::erase(const Key key)
    -> std::optional<ID>
{
    const Index index{ index_from_key(key) };
    if (index >= m_pointers.size()) {
        return std::nullopt;
    }

    const Pointer pointer{ m_pointers[index] };
    const ID      id{ id_from_pointer(pointer) };
    const Version version{ version_from_pointer(pointer) };
    if (id == invalid_id || version_from_key(key) != version) {
        return std::nullopt;
    }

    const Index moved_index = m_indices[id] = m_indices.back();
    m_indices.pop_back();
    m_pointers[moved_index] = make_pointer(id, version_from_pointer(m_pointers[moved_index]));

    if (m_youngest_dead_index != invalid_index) {
        m_pointers[m_youngest_dead_index] =
            make_pointer(index, version_from_pointer(m_pointers[m_youngest_dead_index]));
    }
    m_youngest_dead_index = index;

    if (m_oldest_dead_index == invalid_index) {
        m_oldest_dead_index = index;
    }

    m_pointers[index] = make_pointer(invalid_index, static_cast<Version>(version + 1));

    return id;
}

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
constexpr auto SparseSet<Key_T, version_bit_size_T>::get(const Key key) const -> ID
{
    const Index index{ index_from_key(key) };
    assert(index < m_pointers.size() && "invalid key");

    const ID id{ id_from_pointer(m_pointers[index]) };
    assert(id != invalid_id && "invalid key");

    assert(
        version_from_key(key) == version_from_pointer(m_pointers[index]) && "invalid key"
    );

    return id;
}

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
constexpr auto SparseSet<Key_T, version_bit_size_T>::find(const Key key) const noexcept
    -> std::optional<ID>
{
    const Index index{ index_from_key(key) };
    if (index >= m_pointers.size()) {
        return std::nullopt;
    }

    const Pointer pointer{ m_pointers[index] };
    const ID      id{ id_from_pointer(pointer) };
    const Version version{ version_from_pointer(pointer) };
    if (id == invalid_id || version_from_key(key) != version) {
        return std::nullopt;
    }

    return id;
}

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
constexpr auto SparseSet<Key_T, version_bit_size_T>::contains(const Key key) const noexcept
    -> bool
{
    return find(key).has_value();
}

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
constexpr auto SparseSet<Key_T, version_bit_size_T>::empty() const noexcept -> bool
{
    return m_indices.empty();
}

template <typename Key_T, uint8_t version_bit_size_T>
    requires std::unsigned_integral<Key_T> && (!std::is_const_v<Key_T>)
constexpr auto SparseSet<Key_T, version_bit_size_T>::emplace_pointer(const ID id)
{
    struct PointersPopBackFunctor {
        SparseSet* capture{};

        auto operator()() noexcept
        {
            if (capture != nullptr) {
                capture->m_pointers.pop_back();
            }
        }
    };

    if (m_oldest_dead_index == invalid_index) {
        const Index   index{ static_cast<Index>(m_pointers.size()) };
        const Version version{};

        m_pointers.push_back(make_pointer(id, version));

        return std::make_tuple(
            index, version, util::ScopeGuard{ PointersPopBackFunctor{ this } }
        );
    }

    const Pointer old_pointer{ m_pointers[m_oldest_dead_index] };
    const Index   index{ m_oldest_dead_index };
    const Version version{ version_from_pointer(old_pointer) };

    m_oldest_dead_index = id_from_pointer(old_pointer);
    if (m_youngest_dead_index == index) {
        m_youngest_dead_index = invalid_index;
    }

    m_pointers[index] = make_pointer(id, version);

    return std::make_tuple(
        index, version, util::ScopeGuard{ PointersPopBackFunctor{} }
    );
}

namespace util {

export template <typename Key_T, uint8_t version_bit_size_T = sizeof(Key_T) * 2>
using SparseSet = ::SparseSet<Key_T, version_bit_size_T>;

}   // namespace util

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

using Key = uint32_t;
constexpr Key missing_key{ std::numeric_limits<Key>::max() };

static_assert(
    [] {
        util::SparseSet<Key> sparse_set;
        const auto [key, id]{ sparse_set.emplace() };

        assert(sparse_set.get(key) == id);

        return true;
    }(),
    "get test failed"
);

static_assert(
    [] {
        util::SparseSet<Key> sparse_set;
        const auto [key, id]{ sparse_set.emplace() };

        assert(sparse_set.find(key).value() == id);

        return true;
    }(),
    "find contained test failed"
);

static_assert(
    [] {
        const util::SparseSet<Key> sparse_set;

        assert(!sparse_set.find(missing_key).has_value());

        return true;
    }(),
    "find missing test failed"
);

static_assert(
    [] {
        util::SparseSet<Key> sparse_set;
        const auto [key, _]{ sparse_set.emplace() };

        assert(sparse_set.erase(key));
        assert(!sparse_set.find(key).has_value());

        return true;
    }(),
    "erase contained value test failed"
);

static_assert(
    [] {
        util::SparseSet<Key> sparse_set;

        assert(!sparse_set.erase(missing_key));

        return true;
    }(),
    "erase missing value test failed"
);

static_assert(
    [] {
        util::SparseSet<Key> sparse_set;
        const auto [old_key, _]{ sparse_set.emplace() };
        sparse_set.erase(old_key);
        const auto [new_key, _]{ sparse_set.emplace() };

        assert(!sparse_set.find(old_key).has_value());
        assert(sparse_set.contains(new_key));

        return true;
    }(),
    "version test failed"
);

static_assert(
    [] {
        util::SparseSet<Key> sparse_set;
        const auto [first_key, _]{ sparse_set.emplace() };
        const auto [second_key, _]{ sparse_set.emplace() };
        sparse_set.erase(second_key);
        const auto [third_key, _]{ sparse_set.emplace() };
        sparse_set.erase(first_key);

        assert(sparse_set.contains(third_key));

        return true;
    }(),
    "complex erase test failed"
);

#endif
