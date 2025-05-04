#include <type_traits>

import utility.meta.type_traits.type_list.type_list_replace;

namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(std::is_same_v<
              util::meta::type_list_replace_t<TypeList<int, float, int>, int, long>,
              TypeList<long, float, long>>);

static_assert(std::is_same_v<
              util::meta::type_list_replace_t<TypeList<int, float, int>, int, long>,
              TypeList<long, float, long>>);
