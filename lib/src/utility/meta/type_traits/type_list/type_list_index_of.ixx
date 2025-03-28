module;

#include <tuple>
#include <type_traits>

export module utility.meta.type_traits.type_list.type_list_index_of;

template <typename, typename, size_t>
struct type_list_index_of_impl;

template <
    template <typename...> typename TypeList_T,
    typename First_T,
    typename... Ts,
    typename T,
    size_t index_T>
struct type_list_index_of_impl<TypeList_T<First_T, Ts...>, T, index_T> {
    constexpr static size_t value =
        type_list_index_of_impl<TypeList_T<Ts...>, T, index_T + 1>::value;
};

template <
    template <typename...> typename TypeList_T,
    typename First_T,
    typename... Ts,
    size_t index_T>
struct type_list_index_of_impl<TypeList_T<First_T, Ts...>, First_T, index_T> {
    constexpr static size_t value = index_T;
};

namespace util::meta {

template <typename TypeList_T, typename T>
struct type_list_index_of {
    constexpr static size_t value = ::type_list_index_of_impl<TypeList_T, T, 0>::value;
};

export template <typename TypeList_T, typename T>
constexpr inline size_t type_list_index_of_v = type_list_index_of<TypeList_T, T>::value;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(util::meta::type_list_index_of_v<std::tuple<int, float, long>, int> == 0);
static_assert(util::meta::type_list_index_of_v<std::tuple<int, float, long>, float> == 1);
static_assert(util::meta::type_list_index_of_v<std::tuple<int, float, long>, long> == 2);

#endif
