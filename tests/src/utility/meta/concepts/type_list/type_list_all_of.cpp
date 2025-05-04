#include <type_traits>

import utility.meta.concepts.type_list.type_list_all_of;

namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(util::meta::type_list_all_of_c<TypeList<>, std::is_scalar>);
static_assert(util::meta::type_list_all_of_c<TypeList<int>, std::is_scalar>);
static_assert(util::meta::type_list_all_of_c<TypeList<int, float>, std::is_scalar>);
