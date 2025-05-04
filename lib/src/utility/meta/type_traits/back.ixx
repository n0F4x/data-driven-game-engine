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
