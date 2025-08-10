#include <type_traits>

import ddge.utility.meta.type_traits.type_list.type_list_drop_front;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

static_assert(std::is_same_v<
              ddge::util::meta::type_list_drop_front_t<TypeList<int, float>>,
              TypeList<float>>);
static_assert(std::is_same_v<
              ddge::util::meta::type_list_drop_front_t<TypeList<float>>,
              TypeList<>>);
