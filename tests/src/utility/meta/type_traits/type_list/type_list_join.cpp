#include <type_traits>

import utility.meta.type_traits.type_list.type_list_join;

namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(std::is_same_v<
              util::meta::type_list_join_t<TypeList<TypeList<int>, TypeList<float>>>,
              TypeList<int, float>>);
