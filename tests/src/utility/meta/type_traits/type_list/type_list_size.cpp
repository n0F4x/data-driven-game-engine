import ddge.utility.meta.type_traits.type_list.type_list_size;

namespace {

template <typename...>
struct TypeList {};

}   // namespace

static_assert(ddge::util::meta::type_list_size_v<TypeList<>> == 0);
static_assert(ddge::util::meta::type_list_size_v<TypeList<int, float>> == 2);
