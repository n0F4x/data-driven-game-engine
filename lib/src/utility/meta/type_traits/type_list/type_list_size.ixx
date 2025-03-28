module;

#include <cstdint>

export module utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <typename TypeList_T>
struct type_list_size;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_size<TypeList_T<Ts...>> {
    constexpr static size_t value = sizeof...(Ts);
};

export template <typename TypeList_T>
constexpr inline size_t type_list_size_v = type_list_size<TypeList_T>::value;

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

// TODO: remove unnamed namespace when Clang allows it
namespace {
template <typename...>
struct TypeList {};
}   // namespace

static_assert(util::meta::type_list_size_v<TypeList<>> == 0);
static_assert(util::meta::type_list_size_v<TypeList<int, float>> == 2);

#endif
