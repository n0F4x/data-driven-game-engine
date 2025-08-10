#include <type_traits>

import ddge.utility.meta.type_traits.type_list.type_list_filter;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

static_assert(std::is_same_v<
              ddge::util::meta::type_list_filter_t<TypeList<float>, std::is_integral>,
              TypeList<>>);
static_assert(std::is_same_v<
              ddge::util::meta::type_list_filter_t<TypeList<int, float>, std::is_integral>,
              TypeList<int>>);
static_assert(std::is_same_v<
              ddge::util::meta::type_list_filter_t<TypeList<int, float, long>, std::is_integral>,
              TypeList<int, long>>);
