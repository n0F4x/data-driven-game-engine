module;

#include <type_traits>

export module utility.meta.type_traits.back;

namespace util::meta {

export template <typename... Ts>
    requires(sizeof...(Ts) != 0)
struct back : std::type_identity<Ts...[sizeof...(Ts) - 1]> {};

export template <typename... Ts>
    requires(sizeof...(Ts) != 0)
using back_t = typename back<Ts...>::type;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

static_assert(std::is_same_v<util::meta::back_t<int>, int>);
static_assert(std::is_same_v<util::meta::back_t<int, long>, long>);

#endif
