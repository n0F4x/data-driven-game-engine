import ddge.utility.meta.type_traits.type_list.type_list_index_of;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

static_assert(
    ddge::util::meta::type_list_index_of_v<TypeList<int, float, long>, int> == 0
);
static_assert(
    ddge::util::meta::type_list_index_of_v<TypeList<int, float, long>, float> == 1
);
static_assert(
    ddge::util::meta::type_list_index_of_v<TypeList<int, float, long>, long> == 2
);
