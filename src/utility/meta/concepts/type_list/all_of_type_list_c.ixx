module;

#include <tuple>
#include <type_traits>

export module utility.meta.concepts.type_list.all_of_type_list_c;

namespace util::meta {

template <typename, template <typename> typename>
struct all_of_type_list : std::false_type {};

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Predicate_T>
struct all_of_type_list<TypeList_T<Ts...>, Predicate_T> : std::conjunction<Predicate_T<Ts>...> {
};

export template <typename TypeList_T, template <typename> typename Predicate_T>
concept all_of_type_list_c =  all_of_type_list<TypeList_T, Predicate_T>::value;

}   // namespace util::meta
