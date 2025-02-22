module;

#include <type_traits>

export module utility.meta.type_traits.select_last;

namespace util::meta {

export template <typename... Ts>
    requires(sizeof...(Ts) != 0)
struct select_last {
    using type = decltype((std::type_identity_t<Ts>{}, ...));
};

export template <typename... Ts>
    requires(sizeof...(Ts) != 0)
using select_last_t = typename select_last<Ts...>::type;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_TESTS

static_assert(std::is_same_v<util::meta::select_last_t<int>, int>);
static_assert(std::is_same_v<util::meta::select_last_t<int, long>, long>);

#endif
