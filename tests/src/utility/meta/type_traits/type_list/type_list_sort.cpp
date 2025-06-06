#include <type_traits>

import utility.meta.type_traits.type_list.type_list_sort;

namespace {
template <typename...>
struct TypeList {};
}   // namespace

template <size_t int_T>
using Int = std::integral_constant<size_t, int_T>;

template <typename Int>
struct Hash {
    constexpr static size_t value = Int::value + 10;
};

static_assert(std::is_same_v<util::meta::type_list_sort_t<TypeList<>, Hash>, TypeList<>>);
static_assert(std::is_same_v<
              util::meta::type_list_sort_t<TypeList<Int<1>, Int<2>, Int<0>>, Hash>,
              TypeList<Int<0>, Int<1>, Int<2>>>);
