module;

#include <bitset>
#include <concepts>
#include <limits>
#include <ranges>
#include <utility>
#include <vector>

export module utility.SparseSet;

import utility.meta.concepts.nothrow_movable;
import utility.meta.type_traits.forward_like;
import utility.meta.uint_at_least;
import utility.OptionalRef;
import utility.ScopeGuard;

namespace util {

template <typename T>
concept key_c = std::unsigned_integral<T> && !std::is_const_v<T>
             && util::meta::nothrow_movable_c<T>;

export template <key_c Key_T, typename T, uint8_t version_bits_T = sizeof(Key_T) * 2>
    requires(sizeof(Key_T) * 8 > version_bits_T)
class SparseSet {
public:
    using Key = Key_T;

    template <typename... Args>
    auto emplace(Args&&... args) -> Key;

    auto erase(Key key) -> bool;

    template <typename Self>
    [[nodiscard]]
    auto get(this Self&&, Key key) -> meta::forward_like_t<T, Self>;

    [[nodiscard]]
    auto find(Key key) -> OptionalRef<T>;
    [[nodiscard]]
    auto find(Key key) const -> OptionalRef<const T>;

private:
    constexpr static uint8_t key_bit_size{ sizeof(Key_T) * 8 };

    constexpr static uint8_t version_bit_size{ version_bits_T };
    constexpr static uint8_t first_version_bit{};
    using Version = meta::uint_at_least_t<version_bit_size>;

    constexpr static uint8_t index_bit_size{ key_bit_size - version_bit_size };
    constexpr static uint8_t first_index_bit{ version_bits_T };
    using Index = meta::uint_at_least_t<index_bit_size>;
    constexpr static Index invalid_index{ std::numeric_limits<Index>::max() };

    using Pointer = Key;

    std::vector<Pointer> m_pointers;
    std::vector<T>       m_values;
    std::vector<Index>   m_ids;
    Index                oldest_dead_id{ invalid_index };
    Index                youngest_dead_id{ invalid_index };

    [[nodiscard]]
    constexpr static auto id_from_key(Key key) noexcept -> Index;
    [[nodiscard]]
    constexpr static auto version_from_key(Key key) noexcept -> Version;
    [[nodiscard]]
    consteval static auto make_key(Index index, Version version) noexcept -> Key;

    [[nodiscard]]
    constexpr static auto index_from_pointer(Pointer pointer) noexcept -> Index;
    [[nodiscard]]
    constexpr static auto version_from_pointer(Pointer pointer) noexcept -> Version;
    [[nodiscard]]
    consteval static auto make_pointer(Index index, Version version) noexcept -> Pointer;
};

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto SparseSet<Key_T, T, version_bits_T>::id_from_key(const Key key) noexcept
    -> Index
{
    constexpr static Key mask{ [] {
        std::bitset<key_bit_size> tmp_mask{};
        for (const auto i : std::views::iota(
                 first_index_bit, static_cast<uint8_t>(first_index_bit + index_bit_size)
             ))
        {
            tmp_mask.set(i);
        }
        return static_cast<Key>(tmp_mask.to_ullong());
    }() };

    return static_cast<Index>(
        (key & mask) >> (key_bit_size - first_index_bit - index_bit_size)
    );
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto SparseSet<Key_T, T, version_bits_T>::version_from_key(
    const Key key
) noexcept -> Version
{
    constexpr static Key mask{ [] {
        std::bitset<key_bit_size> tmp_mask{};
        for (const auto i : std::views::iota(first_version_bit, version_bit_size)) {
            tmp_mask.set(i);
        }
        return static_cast<Key>(tmp_mask.to_ullong());
    }() };

    return static_cast<Version>(
        (key & mask) >> (key_bit_size - first_version_bit - version_bit_size)
    );
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
consteval auto SparseSet<Key_T, T, version_bits_T>::make_key(
    const Index   index,
    const Version version
) noexcept -> Key
{
    return (Key{ index } << (key_bit_size - first_index_bit - index_bit_size))
         + (Key{ version } << (key_bit_size - first_version_bit - version_bit_size));
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto SparseSet<Key_T, T, version_bits_T>::index_from_pointer(
    const Pointer pointer
) noexcept -> Index
{
    static_assert(std::is_same_v<Pointer, Key>);

    return id_from_key(pointer);
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
constexpr auto SparseSet<Key_T, T, version_bits_T>::version_from_pointer(
    const Pointer pointer
) noexcept -> Version
{
    static_assert(std::is_same_v<Pointer, Key>);

    return version_from_key(pointer);
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
consteval auto SparseSet<Key_T, T, version_bits_T>::make_pointer(
    const Index   index,
    const Version version
) noexcept -> Pointer
{
    static_assert(std::is_same_v<Pointer, Key>);

    return make_key(index, version);
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
template <typename... Args>
auto SparseSet<Key_T, T, version_bits_T>::emplace(Args&&... args) -> Key
{
    m_values.emplace_back(std::forward<Args>(args)...);
    ScopeGuard  _{ [&] { m_values.pop_back(); } };
    const Index new_index{ m_values.size() - 1 };

    m_ids.emplace_back(new_index);
    ScopeGuard _{ [&] { m_ids.pop_back(); } };

    if (oldest_dead_id == invalid_index) {
        const Index   new_id{ m_pointers.size() };
        const Version new_version{};
        m_pointers.push_back(make_pointer(new_index, new_version));
        return make_key(new_id, new_version);
    }

    return [this, new_index] noexcept {
        const Pointer old_pointer{ m_pointers[oldest_dead_id] };
        const Index   new_id{ oldest_dead_id };
        const Version new_version{ version_from_pointer(old_pointer) + 1 };
        oldest_dead_id = index_from_pointer(old_pointer);
        if (youngest_dead_id == new_id) {
            youngest_dead_id = invalid_index;
        }
        m_pointers[new_id] = make_pointer(new_index, new_version);
        return make_key(new_id, new_version);
    }();
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
auto SparseSet<Key_T, T, version_bits_T>::erase(const Key key) -> bool
{
    const Index id{ id_from_key(key) };
    if (id >= m_pointers.size()) {
        return false;
    }

    const Pointer pointer{ m_pointers[id] };
    const Index   index{ index_from_pointer(pointer) };
    const Version version{ version_from_pointer(pointer) };
    if (index == invalid_index || version_from_key(key) != version) {
        return false;
    }

    m_values[index] = std::move(m_values.back());
    m_values.pop_back();
    const Index moved_id = m_ids[index] = std::move(m_ids.back());
    m_ids.pop_back();
    m_pointers[index_from_key(moved_id)] = pointer;

    if (youngest_dead_id != invalid_index) {
        m_pointers[youngest_dead_id] =
            make_pointer(id, version_from_pointer(m_pointers[youngest_dead_id]));
    }
    youngest_dead_id = id;
    m_pointers[id]   = make_pointer(invalid_index, version);

    return true;
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
template <typename Self>
auto SparseSet<Key_T, T, version_bits_T>::get(this Self&& self, const Key key)
    -> meta::forward_like_t<T, Self>
{
    assert(
        version_from_key(key) == version_from_pointer(self.m_pointers[id_from_key(key)])
    );

    return std::forward_like<Self>(
        self.m_values.at(index_from_pointer(self.m_pointers[id_from_key(key)]))
    );
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
auto SparseSet<Key_T, T, version_bits_T>::find(const Key key) -> OptionalRef<T>
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

    return OptionalRef<T>{ m_values[index] };
}

template <key_c Key_T, typename T, uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
auto SparseSet<Key_T, T, version_bits_T>::find(const Key key) const
    -> OptionalRef<const T>
{
    return const_cast<SparseSet>(*this).find(key);
}

}   // namespace util
