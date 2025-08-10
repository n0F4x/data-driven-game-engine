#include <type_traits>

import ddge.utility.meta.type_traits.type_list.type_list_sort;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

template <std::size_t int_T>
using Int = std::integral_constant<std::size_t, int_T>;

template <typename Int>
struct Hash {
    constexpr static std::size_t value = Int::value + 10;
};

static_assert(std::is_same_v<
              ddge::util::meta::type_list_sort_t<TypeList<>, Hash>,
              TypeList<>>);
static_assert(std::is_same_v<
              ddge::util::meta::type_list_sort_t<TypeList<Int<1>, Int<2>, Int<0>>, Hash>,
              TypeList<Int<0>, Int<1>, Int<2>>>);
