import utility.meta.type_traits.type_list.is_type_list;

namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(util::meta::is_type_list_v<TypeList<>>);
static_assert(util::meta::is_type_list_v<TypeList<int, float>>);
