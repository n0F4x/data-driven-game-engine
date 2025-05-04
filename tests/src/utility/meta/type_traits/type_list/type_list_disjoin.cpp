#include <type_traits>

import utility.meta.type_traits.type_list.type_list_disjoin;

namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(std::is_same_v<
              util::meta::type_list_disjoin_t<TypeList<int, float>>,
              TypeList<TypeList<int>, TypeList<float>>>);
