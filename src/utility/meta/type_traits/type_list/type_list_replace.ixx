module;

#include <type_traits>

export module utility.meta.type_traits.type_list.type_list_replace;

import utility.meta.concepts.type_list.type_list_c;

#ifdef ENGINE_ENABLE_STATIC_TESTS
import utility.TypeList;
#endif

namespace util::meta {

export template <typename TypeList_T, typename OldType_T, typename NewType_T>
struct type_list_replace;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    typename OldType_T,
    typename NewType_T>
struct type_list_replace<TypeList_T<Ts...>, OldType_T, NewType_T> {
    using type =
        TypeList_T<std::conditional_t<std::is_same_v<Ts, OldType_T>, NewType_T, Ts>...>;
};

export template <type_list_c TypeList_T, typename OldType_T, typename NewType_T>
using type_list_replace_t =
    typename type_list_replace<TypeList_T, OldType_T, NewType_T>::type;

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(std::is_same_v<
              util::meta::type_list_replace_t<util::TypeList<int, float, int>, int, long>,
              util::TypeList<long, float, long>>);

static_assert(std::is_same_v<
              util::meta::type_list_replace_t<util::TypeList<int, float, int>, int, long>,
              util::TypeList<long, float, long>>);

#endif
