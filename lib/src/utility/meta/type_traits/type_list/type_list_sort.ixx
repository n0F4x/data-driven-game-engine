module;

#include <algorithm>
#include <array>

export module ddge.utility.meta.type_traits.type_list.type_list_sort;

import ddge.utility.meta.concepts.type_list.type_list;
import ddge.utility.meta.concepts.type_list.type_list_all_of;

template <template <typename> typename Hash_T>
struct hash_has_valid_member_value {
    template <typename T>
    struct type {
        constexpr static bool value = std::
            convertible_to<std::remove_cvref_t<decltype(Hash_T<T>::value)>, std::size_t>;
    };
};

template <typename TypeList_T, template <typename> typename Hash_T>
struct type_list_sort_impl;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Hash_T>
struct type_list_sort_impl<TypeList_T<Ts...>, Hash_T> {
    template <std::array<std::size_t, sizeof...(Ts)>, typename>
    struct helper;

    template <std::array<std::size_t, sizeof...(Ts)> hashes_T, std::size_t... Is>
    struct helper<hashes_T, std::index_sequence<Is...>> {
        constexpr static std::array sorted_indices = [] {
            std::array<std::pair<std::size_t, std::size_t>, sizeof...(Ts)> result{
                std::pair{ Is, hashes_T[Is] }
                ...
            };
            std::ranges::sort(result, {}, &std::pair<std::size_t, std::size_t>::second);
            return std::array<std::size_t, sizeof...(Ts)>{ (result[Is].first)... };
        }();

        template <std::size_t I>
        using new_type_t = Ts...[sorted_indices[I]];

        using type = TypeList_T<new_type_t<Is>...>;
    };

    using type = typename helper<
        std::array<std::size_t, sizeof...(Ts)>{ Hash_T<Ts>::value... },
        std::make_index_sequence<sizeof...(Ts)>>::type;
};

namespace ddge::util::meta {

export template <type_list_c TypeList_T, template <typename> typename Hash_T>
    requires type_list_all_of_c<
        TypeList_T,
        ::hash_has_valid_member_value<Hash_T>::template type>
struct type_list_sort {
    using type = typename ::type_list_sort_impl<TypeList_T, Hash_T>::type;
};

export template <type_list_c TypeList_T, template <typename> typename Hash_T>
    requires type_list_all_of_c<TypeList_T, hash_has_valid_member_value<Hash_T>::template type>
using type_list_sort_t = typename type_list_sort<TypeList_T, Hash_T>::type;

}   // namespace ddge::util::meta
