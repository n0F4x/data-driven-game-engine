#include <type_traits>

import utility.meta.type_traits.type_list.type_list_unique;

namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(std::is_same_v<util::meta::type_list_unique_t<TypeList<>>, TypeList<>>);
static_assert(std::is_same_v<
              util::meta::type_list_unique_t<TypeList<int, float, int>>,
              TypeList<int, float>>);
