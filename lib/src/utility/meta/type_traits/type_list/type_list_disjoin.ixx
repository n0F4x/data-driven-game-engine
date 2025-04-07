module;

#ifdef ENGINE_ENABLE_STATIC_TESTS
  #include <type_traits>
#endif

export module utility.meta.type_traits.type_list.type_list_disjoin;

namespace util::meta {

export template <typename TypeList_T>
struct type_list_disjoin;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_disjoin<TypeList_T<Ts...>> {
    using type = TypeList_T<TypeList_T<Ts>...>;
};

export template <typename TypeList_T>
using type_list_disjoin_t = typename type_list_disjoin<TypeList_T>::type;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(std::is_same_v<
              util::meta::type_list_disjoin_t<TypeList<int, float>>,
              TypeList<TypeList<int>, TypeList<float>>>);

#endif
