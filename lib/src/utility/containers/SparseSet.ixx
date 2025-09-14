module;

#include <bitset>
#include <concepts>
#include <limits>
#include <ranges>
#include <type_traits>
#include <vector>

#include "utility/contracts_macros.hpp"

export module ddge.utility.containers.SparseSet;

import ddge.utility.contracts;
import ddge.utility.meta.uint_at_least;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.ScopeGuard;
import ddge.utility.containers.Strong;

template <typename T>
concept key_c = std::unsigned_integral<T> && !std::is_const_v<T>
             && !std::is_volatile_v<T>;

template <key_c Key_T, uint8_t version_bit_size_T = sizeof(Key_T) * 2>
class SparseSetTraits {
public:
    using Key = Key_T;
    constexpr static uint8_t key_bit_size{ sizeof(Key) * 8 };

    constexpr static uint8_t version_bit_size{ version_bit_size_T };
    constexpr static uint8_t first_version_bit{};
    using Version = ddge::util::meta::uint_at_least_t<version_bit_size>;
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
    using Index = ddge::util::meta::uint_at_least_t<index_bit_size>;
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
        return Key{ ((Key{ index } << (key_bit_size - first_index_bit - index_bit_size))
                     & index_mask)
                    + ((Key{ version }
                        << (key_bit_size - first_version_bit - version_bit_size))
                       & version_mask) };
    }

    [[nodiscard]]
    constexpr static auto id_from_pointer(const Pointer pointer) noexcept -> ID
    {
        return index_from_key(Key{ pointer });
    }

    [[nodiscard]]
    constexpr static auto version_from_pointer(const Pointer pointer) noexcept -> Version
    {
        return version_from_key(Key{ pointer });
    }

    [[nodiscard]]
    constexpr static auto make_pointer(const ID id, const Version version) noexcept
        -> Pointer
    {
        return make_key(id, version);
    }
};

/**
 * A SparseSet contains a set of generated keys.
 * Each key is mapped to a sequential identifier that may change on erasure.
 * Upon erasing a key, the highest identifier will be erased with it.
 * The identifier of the erased key is remapped to the key of the erased indentifier.
 *
 * This mechanism allows for creating a dense array of values that can be indexed by such
 * identifiers.
 * When pushing a new value to the dense array, generate a key and id for it using a
 * SparseSet.
 * To erase a value from the dense array, first erase its key from the SparseSet, then
 * swap it with the last contained value and pop it.
 * This way, you can keep using the ids as indices to your array, and the keys will stay
 * stable.
 * This data structure is also called a slot map.
 */
template <key_c Key_T, uint8_t version_bit_size_T = sizeof(Key_T) * 2>
class SparseSet : SparseSetTraits<Key_T, version_bit_size_T> {
    using Base = SparseSetTraits<Key_T, version_bit_size_T>;

public:
    using Key = typename Base::Key;
    using ID  = typename Base::ID;

    constexpr static uint8_t version_bit_size{ Base::version_bit_size };

    [[nodiscard]]
    constexpr auto next_key() const noexcept -> Key;
    [[nodiscard]]
    constexpr auto next_id() const noexcept -> ID;

    constexpr auto emplace() -> std::pair<Key, ID>;
    constexpr auto erase(Key key) -> std::optional<ID>;
    constexpr auto remove(Key key) -> ID;

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
    constexpr auto next_index() const noexcept -> Index;
    [[nodiscard]]
    constexpr auto next_pointer() const noexcept -> Pointer;

    [[nodiscard]]
    constexpr auto emplace_pointer(ID id);
};

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::next_key() const noexcept -> Key
{
    return make_key(next_index(), version_from_pointer(next_pointer()));
}

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::next_id() const noexcept -> ID
{
    return static_cast<ID>(m_indices.size());
}

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::emplace() -> std::pair<Key, ID>
{
    const ID id{ next_id() };

    const auto [index, version, scope_guard] = emplace_pointer(id);

    m_indices.push_back(index);

    return std::make_pair(make_key(index, version), id);
}

template <key_c Key_T, uint8_t version_bit_size_T>
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
    m_pointers[moved_index] =
        make_pointer(id, version_from_pointer(m_pointers[moved_index]));

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

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::remove(const Key key) -> ID
{
    const std::optional<ID> optional_id{ erase(key) };
    PRECOND(optional_id.has_value(), "key must be contained");
    return *optional_id;
}

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::get(const Key key) const -> ID
{
    const Index index{ index_from_key(key) };
    PRECOND(index < m_pointers.size(), "invalid key");

    const ID id{ id_from_pointer(m_pointers[index]) };
    PRECOND(id != invalid_id, "invalid key");

    PRECOND(
        version_from_key(key) == version_from_pointer(m_pointers[index]), "invalid key"
    );

    return id;
}

template <key_c Key_T, uint8_t version_bit_size_T>
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

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::contains(const Key key) const noexcept
    -> bool
{
    return find(key).has_value();
}

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::empty() const noexcept -> bool
{
    return m_indices.empty();
}

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::next_index() const noexcept -> Index
{
    if (m_oldest_dead_index == invalid_index) {
        return static_cast<Index>(m_pointers.size());
    }

    return m_oldest_dead_index;
}

template <key_c Key_T, uint8_t version_bit_size_T>
constexpr auto SparseSet<Key_T, version_bit_size_T>::next_pointer() const noexcept
    -> Pointer
{
    if (m_oldest_dead_index == invalid_index) {
        const Version version{};
        return make_pointer(next_id(), version);
    }

    const Pointer oldest_pointer{ m_pointers[m_oldest_dead_index] };
    const Version version{ version_from_pointer(oldest_pointer) };

    return make_pointer(next_id(), version);
}

template <key_c Key_T, uint8_t version_bit_size_T>
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
            index, version, ddge::util::ScopeGuard{ PointersPopBackFunctor{ this } }
        );
    }

    const Pointer oldest_pointer{ m_pointers[m_oldest_dead_index] };
    const Index   index{ m_oldest_dead_index };
    const Version version{ version_from_pointer(oldest_pointer) };

    m_oldest_dead_index = id_from_pointer(oldest_pointer);
    if (m_youngest_dead_index == index) {
        m_youngest_dead_index = invalid_index;
    }

    m_pointers[index] = make_pointer(id, version);

    return std::make_tuple(
        index, version, ddge::util::ScopeGuard{ PointersPopBackFunctor{} }
    );
}

namespace ddge::util {

export template <key_c Key_T, uint8_t version_bit_size_T = sizeof(Key_T) * 2>
using SparseSet = ::SparseSet<Key_T, version_bit_size_T>;

}   // namespace ddge::util
