#include <type_traits>

import ddge.utility.meta.type_traits.type_list.type_list_concat;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

static_assert(std::is_same_v<ddge::util::meta::type_list_concat_t<TypeList<int>>, TypeList<int>>);

static_assert(std::is_same_v<
              ddge::util::meta::type_list_concat_t<TypeList<int>, TypeList<float>>,
              TypeList<int, float>>);
