#include <type_traits>

import ddge.utility.meta.type_traits.type_list.type_list_drop_back;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

static_assert(std::is_same_v<
              ddge::util::meta::type_list_drop_back_t<TypeList<int, float>>,
              TypeList<int>>);
static_assert(
    std::is_same_v<ddge::util::meta::type_list_drop_back_t<TypeList<int>>, TypeList<>>
);
