module;

#include <bitset>
#include <cstdint>
#include <ranges>

export module utility.containers.sparse_set.SparseSetBase;

import utility.containers.sparse_set.key_c;
import utility.meta.uint_at_least;

namespace util::sparse_set {

export template <key_c Key_T, uint8_t version_bit_size_T>
    requires(sizeof(Key_T) * 8 > version_bit_size_T)
class SparseSetBase {
public:
    using Key = Key_T;
    constexpr static uint8_t key_bit_size{ sizeof(Key) * 8 };

    constexpr static uint8_t version_bit_size{ version_bit_size_T };
    constexpr static uint8_t first_version_bit{};
    using Version = meta::uint_at_least_t<version_bit_size>;
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
    using Index = meta::uint_at_least_t<index_bit_size>;
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

    [[nodiscard]]
    constexpr static auto id_from_key(Key key) noexcept -> Index;
    [[nodiscard]]
    constexpr static auto version_from_key(Key key) noexcept -> Version;
    [[nodiscard]]
    constexpr static auto make_key(Index index, Version version) noexcept -> Key;

    [[nodiscard]]
    constexpr static auto index_from_pointer(Pointer pointer) noexcept -> Index;
    [[nodiscard]]
    constexpr static auto version_from_pointer(Pointer pointer) noexcept -> Version;
    [[nodiscard]]
    constexpr static auto make_pointer(Index index, Version version) noexcept -> Pointer;
};

}   // namespace util::sparse_set

template <util::sparse_set::key_c Key_T, uint8_t version_bit_size_T>
    requires(sizeof(Key_T) * 8 > version_bit_size_T)
constexpr auto util::sparse_set::SparseSetBase<Key_T, version_bit_size_T>::id_from_key(
    const Key key
) noexcept -> Index
{
    return static_cast<Index>(
        (key & index_mask) >> (key_bit_size - first_index_bit - index_bit_size)
    );
}

template <util::sparse_set::key_c Key_T, uint8_t version_bit_size_T>
    requires(sizeof(Key_T) * 8 > version_bit_size_T)
constexpr auto util::sparse_set::SparseSetBase<Key_T, version_bit_size_T>::
    version_from_key(const Key key) noexcept -> Version
{
    return static_cast<Version>(
        (key & version_mask) >> (key_bit_size - first_version_bit - version_bit_size)
    );
}

template <util::sparse_set::key_c Key_T, uint8_t version_bit_size_T>
    requires(sizeof(Key_T) * 8 > version_bit_size_T)
constexpr auto util::sparse_set::SparseSetBase<Key_T, version_bit_size_T>::make_key(
    const Index   index,
    const Version version
) noexcept -> Key
{
    return ((Key{ index } << (key_bit_size - first_index_bit - index_bit_size))
            & index_mask)
         + ((Key{ version } << (key_bit_size - first_version_bit - version_bit_size))
            & version_mask);
}

template <util::sparse_set::key_c Key_T, uint8_t version_bit_size_T>
    requires(sizeof(Key_T) * 8 > version_bit_size_T)
constexpr auto util::sparse_set::SparseSetBase<Key_T, version_bit_size_T>::
    index_from_pointer(const Pointer pointer) noexcept -> Index
{
    static_assert(std::is_same_v<Pointer, Key>);

    return id_from_key(pointer);
}

template <util::sparse_set::key_c Key_T, uint8_t version_bit_size_T>
    requires(sizeof(Key_T) * 8 > version_bit_size_T)
constexpr auto util::sparse_set::SparseSetBase<Key_T, version_bit_size_T>::
    version_from_pointer(const Pointer pointer) noexcept -> Version
{
    static_assert(std::is_same_v<Pointer, Key>);

    return version_from_key(pointer);
}

template <util::sparse_set::key_c Key_T, uint8_t version_bit_size_T>
    requires(sizeof(Key_T) * 8 > version_bit_size_T)
constexpr auto util::sparse_set::SparseSetBase<Key_T, version_bit_size_T>::make_pointer(
    const Index   index,
    const Version version
) noexcept -> Pointer
{
    static_assert(std::is_same_v<Pointer, Key>);

    return make_key(index, version);
}
