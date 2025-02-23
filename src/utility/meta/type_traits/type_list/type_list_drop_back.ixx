#ifdef ENGINE_ENABLE_STATIC_TESTS
module;
  #include <type_traits>
#endif

export module utility.meta.type_traits.type_list.type_list_drop_back;

import utility.meta.type_traits.type_list.type_list_take;

#ifdef ENGINE_ENABLE_STATIC_TESTS
import utility.TypeList;
#endif

namespace util::meta {

export template <typename TypeList_T>
struct type_list_drop_back;

template <template <typename...> typename TypeList_T, typename... Ts>
    requires(sizeof...(Ts) >= 1)
struct type_list_drop_back<TypeList_T<Ts...>>
    : type_list_take<TypeList_T<Ts...>, sizeof...(Ts) - 1> {};

export template <typename TypeList_T>
using type_list_drop_back_t = typename type_list_drop_back<TypeList_T>::type;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(std::is_same_v<
              util::meta::type_list_drop_back_t<util::TypeList<int, float>>,
              util::TypeList<int>>);
static_assert(std::is_same_v<
              util::meta::type_list_drop_back_t<util::TypeList<int>>,
              util::TypeList<>>);

#endif
