module;

#include <type_traits>

export module ddge.util.meta.type_traits.type_list.type_list_filter;

import ddge.util.meta.type_traits.type_list.type_list_concat;

namespace ddge::util::meta {

export template <typename TypeList_T, template <typename> typename Predicate_T>
struct type_list_filter;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Predicate_T>
struct type_list_filter<TypeList_T<Ts...>, Predicate_T> {
    using type = type_list_concat_t<
        std::conditional_t<Predicate_T<Ts>::value, TypeList_T<Ts>, TypeList_T<>>...>;
};

export template <typename TypeList_T, template <typename> typename Predicate_T>
using type_list_filter_t = typename type_list_filter<TypeList_T, Predicate_T>::type;

}   // namespace ddge::util::meta
