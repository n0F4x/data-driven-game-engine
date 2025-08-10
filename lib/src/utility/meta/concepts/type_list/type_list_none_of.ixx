module;

#include <type_traits>

export module ddge.utility.meta.concepts.type_list.type_list_none_of;

import ddge.utility.meta.concepts.type_list.type_list;

namespace ddge::util::meta {

template <typename, template <typename> typename>
struct type_list_none_of : std::false_type {};

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Predicate_T>
struct type_list_none_of<TypeList_T<Ts...>, Predicate_T>
    : std::conjunction<std::negation<Predicate_T<Ts>>...> {};

export template <typename TypeList_T, template <typename> typename Predicate_T>
concept type_list_none_of_c = type_list_c<TypeList_T>
                           && type_list_none_of<TypeList_T, Predicate_T>::value;

}   // namespace ddge::util::meta
