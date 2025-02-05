module;

#include <type_traits>

export module utility.type_traits.type_list.type_list_filter;

import utility.type_traits.type_list.type_list_concat;
import utility.type_traits.type_list.type_list_drop;
import utility.type_traits.type_list.type_list_front;

namespace util::meta {

template <
    typename FilteredTypeList_T,
    typename T,
    typename TypeList_T,
    template <typename> typename Predicate_T>
struct type_list_filter_helper;

template <
    typename FilteredTypeList_T,
    typename T,
    template <typename...> typename TypeList_T,
    template <typename> typename Predicate_T>
struct type_list_filter_helper<FilteredTypeList_T, T, TypeList_T<>, Predicate_T> {
    using type = type_list_concat_t<
        FilteredTypeList_T,
        std::conditional_t<Predicate_T<T>::value, TypeList_T<T>, TypeList_T<>>,
        TypeList_T<>>;
};

template <
    typename FilteredTypeList_T,
    typename T,
    template <typename...> typename TypeList_T,
    typename Rest_T,
    typename... Rest_Ts,
    template <typename> typename Predicate_T>
struct type_list_filter_helper<
    FilteredTypeList_T,
    T,
    TypeList_T<Rest_T, Rest_Ts...>,
    Predicate_T> {
    using type = typename type_list_filter_helper<
        type_list_concat_t<
            FilteredTypeList_T,
            std::conditional_t<Predicate_T<T>::value, TypeList_T<T>, TypeList_T<>>>,
        type_list_front_t<TypeList_T<Rest_T, Rest_Ts...>>,
        type_list_drop_t<TypeList_T<Rest_T, Rest_Ts...>, 1>,
        Predicate_T>::type;
};

export template <typename TypeList_T, template <typename> typename Predicate_T>
struct type_list_filter;

template <template <typename...> typename TypeList_T, template <typename> typename Predicate_T>
struct type_list_filter<TypeList_T<>, Predicate_T> {
    using type = TypeList_T<>;
};

template <
    template <typename...> typename TypeList_T,
    typename T,
    typename... Ts,
    template <typename> typename Predicate_T>
struct type_list_filter<TypeList_T<T, Ts...>, Predicate_T> {
    using type =
        typename type_list_filter_helper<TypeList_T<>, T, TypeList_T<Ts...>, Predicate_T>::
            type;
};

export template <typename TypeList_T, template <typename> typename Predicate_T>
using type_list_filter_t = typename type_list_filter<TypeList_T, Predicate_T>::type;

}   // namespace util::meta
