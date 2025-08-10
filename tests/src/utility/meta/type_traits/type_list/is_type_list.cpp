import ddge.utility.meta.type_traits.type_list.is_type_list;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

static_assert(ddge::util::meta::is_type_list_v<TypeList<>>);
static_assert(ddge::util::meta::is_type_list_v<TypeList<int, float>>);
