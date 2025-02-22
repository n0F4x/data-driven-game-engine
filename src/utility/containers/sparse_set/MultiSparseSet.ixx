module;

#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>

export module utility.containers.sparse_set.MultiSparseSet;

import utility.containers.sparse_set.key_c;
import utility.containers.sparse_set.SparseSetBase;
import utility.containers.sparse_set.value_c;

import utility.meta.type_traits.forward_like;

namespace util::sparse_set {

export template <
    typename Key_T,
    typename TypeList_T,
    uint8_t version_bits_T = sizeof(Key_T) * 2>
class MultiSparseSet;

export template <
    key_c Key_T,
    template <typename...> typename TypeList_T,
    value_c... Ts,
    uint8_t version_bits_T>
    requires(sizeof(Key_T) * 8 > version_bits_T)
class MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>
    : protected SparseSetBase<Key_T, version_bits_T> {
public:
    using Key = Key_T;

    template <typename... Us>
        requires(std::is_constructible_v<Ts, Us> && ...)
    constexpr auto emplace(Us&&... values) -> Key;

    constexpr auto erase(Key key) -> std::optional<std::tuple<Ts...>>;

    template <typename Self>
    [[nodiscard]]
    constexpr auto get(this Self&&, Key key)
        -> std::tuple<meta::forward_like_t<Ts, Self>...>;

    [[nodiscard]]
    constexpr auto find(Key key) -> std::optional<std::tuple<Ts&...>>;
    [[nodiscard]]
    constexpr auto find(Key key) const -> std::optional<std::tuple<const Ts&...>>;

private:
    using Base    = SparseSetBase<Key_T, version_bits_T>;
    using Index   = typename Base::Index;
    using Pointer = typename Base::Pointer;
    using Version = typename Base::Version;

    using Base::invalid_index;

    std::vector<Pointer>           m_pointers;
    std::tuple<std::vector<Ts>...> m_value_containers;
    std::vector<Index>             m_ids;
    Index                          oldest_dead_id{ invalid_index };
    Index                          youngest_dead_id{ invalid_index };
};

}   // namespace util::sparse_set
//
// template <
//     util::sparse_set::key_c Key_T,
//     template <typename...> class TypeList_T,
//     util::sparse_set::value_c... Ts,
//     uint8_t version_bits_T>
//     requires(sizeof(Key_T) * 8 > version_bits_T)
// template <typename... Us>
//     requires(std::is_constructible_v<Ts, Us> && ...)
// constexpr auto
//     util::sparse_set::MultiSparseSet<Key_T, TypeList_T<Ts...>, version_bits_T>::emplace(
//         Us&&... values
//     ) -> Key
// {
//     m_values.emplace_back(std::forward<Args>(args)...);
//     const Index new_index{ m_values.size() - 1 };
//
//     try {
//         m_ids.emplace_back(new_index);
//     } catch (...) {
//         m_values.pop_back();
//         throw;
//     }
//
//     if (oldest_dead_id == invalid_index) {
//         const Index   new_id{ m_pointers.size() };
//         const Version new_version{};
//         try {
//             m_pointers.push_back(make_pointer(new_index, new_version));
//         } catch (...) {
//             m_values.pop_back();
//             m_ids.pop_back();
//             throw;
//         }
//         return make_key(new_id, new_version);
//     }
//
//     return [this, new_index] noexcept {
//         const Pointer old_pointer{ m_pointers[oldest_dead_id] };
//         const Index   new_id{ oldest_dead_id };
//         const Version new_version{ version_from_pointer(old_pointer) };
//         oldest_dead_id = index_from_pointer(old_pointer);
//         if (youngest_dead_id == new_id) {
//             youngest_dead_id = invalid_index;
//         }
//         m_pointers[new_id] = make_pointer(new_index, new_version);
//         return make_key(new_id, new_version);
//     }();
// }

module :private;

#ifdef ENGINE_ENABLE_TESTS

template <typename...>
struct Values;

struct Dummy {
    double value;
};

template <>
class util::sparse_set::MultiSparseSet<uint32_t, Values<int, float, Dummy>>;

#endif
