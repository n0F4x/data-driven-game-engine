module;

#include <algorithm>
#include <array>

export module utility.meta.type_traits.type_list.type_list_sort;

import utility.meta.concepts.type_list.type_list;
import utility.meta.concepts.type_list.type_list_all;

template <template <typename> typename Hash_T>
struct hash_has_valid_member_value {
    template <typename T>
    struct type {
        constexpr static bool value =
            std::convertible_to<std::remove_cvref_t<decltype(Hash_T<T>::value)>, size_t>;
    };
};

template <typename TypeList_T, template <typename> typename Hash_T>
struct type_list_sort_impl;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Hash_T>
struct type_list_sort_impl<TypeList_T<Ts...>, Hash_T> {
    template <std::array<size_t, sizeof...(Ts)>, typename>
    struct helper;

    template <std::array<size_t, sizeof...(Ts)> hashes_T, size_t... Is>
    struct helper<hashes_T, std::index_sequence<Is...>> {
        constexpr static std::array sorted_indices = [] {
            std::array<std::pair<size_t, size_t>, sizeof...(Ts)> result{
                std::pair{ Is, hashes_T[Is] }
                ...
            };
            std::ranges::sort(result, {}, &std::pair<size_t, size_t>::second);
            return std::array<size_t, sizeof...(Ts)>{ (result[Is].first)... };
        }();

        template <size_t I>
        using new_type_t = Ts...[sorted_indices[I]];

        using type = TypeList_T<new_type_t<Is>...>;
    };

    using type = typename helper<
        std::array<size_t, sizeof...(Ts)>{ Hash_T<Ts>::value... },
        std::make_index_sequence<sizeof...(Ts)>>::type;
};

namespace util::meta {

export template <type_list_c TypeList_T, template <typename> typename Hash_T>
    requires type_list_all_c<TypeList_T, ::hash_has_valid_member_value<Hash_T>::template type>
struct type_list_sort {
    using type = typename ::type_list_sort_impl<TypeList_T, Hash_T>::type;
};

export template <type_list_c TypeList_T, template <typename> typename Hash_T>
    requires type_list_all_c<TypeList_T, hash_has_valid_member_value<Hash_T>::template type>
using type_list_sort_t = typename type_list_sort<TypeList_T, Hash_T>::type;

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

// TODO: remove unnamed namespace when Clang allows it
namespace {
template <typename...>
struct TypeList {};
}   // namespace

template <size_t int_T>
using Int = std::integral_constant<size_t, int_T>;

template <typename Int>
struct Hash {
    constexpr static size_t value = Int::value + 10;
};

static_assert(std::is_same_v<util::meta::type_list_sort_t<TypeList<>, Hash>, TypeList<>>);
static_assert(std::is_same_v<
              util::meta::type_list_sort_t<TypeList<Int<1>, Int<2>, Int<0>>, Hash>,
              TypeList<Int<0>, Int<1>, Int<2>>>);

#endif
