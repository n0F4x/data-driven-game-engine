module;

#include <tuple>
#include <type_traits>

export module utility.meta.concepts.type_list.type_list_all_of;

import utility.meta.concepts.type_list.type_list;

namespace util::meta {

template <typename, template <typename> typename>
struct type_list_all_of : std::false_type {};

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename> typename Predicate_T>
struct type_list_all_of<TypeList_T<Ts...>, Predicate_T>
    : std::conjunction<Predicate_T<Ts>...> {};

export template <typename TypeList_T, template <typename> typename Predicate_T>
concept type_list_all_of_c = type_list_c<TypeList_T>
                       && type_list_all_of<TypeList_T, Predicate_T>::value;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(util::meta::type_list_all_of_c<std::tuple<>, std::is_scalar>);
static_assert(util::meta::type_list_all_of_c<std::tuple<int>, std::is_scalar>);
static_assert(util::meta::type_list_all_of_c<std::tuple<int, float>, std::is_scalar>);

#endif
