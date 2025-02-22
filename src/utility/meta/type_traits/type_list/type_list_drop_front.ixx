#ifdef ENGINE_ENABLE_TESTS
module;
  #include <type_traits>
#endif

export module utility.meta.type_traits.type_list.type_list_drop_front;

import utility.meta.type_traits.type_list.type_list_take;

#ifdef ENGINE_ENABLE_TESTS
import utility.TypeList;
#endif

namespace util::meta {

export template <typename TypeList_T>
struct type_list_drop_front;

template <template <typename...> typename TypeList_T, typename T, typename... Ts>
struct type_list_drop_front<TypeList_T<T, Ts...>> {
    using type = TypeList_T<Ts...>;
};

export template <typename TypeList_T>
using type_list_drop_front_t = typename type_list_drop_front<TypeList_T>::type;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_TESTS

static_assert(std::is_same_v<
              util::meta::type_list_drop_front_t<util::TypeList<int, float>>,
              util::TypeList<float>>);
static_assert(std::is_same_v<
              util::meta::type_list_drop_front_t<util::TypeList<float>>,
              util::TypeList<>>);

#endif
