module;

#include <type_traits>

export module ddge.util.meta.type_traits.back;

namespace ddge::util::meta {

export template <typename... Ts>
    requires(sizeof...(Ts) != 0)
struct back : std::type_identity<Ts... [sizeof...(Ts) - 1]> {};

export template <typename... Ts>
    requires(sizeof...(Ts) != 0)
using back_t = Ts...[sizeof...(Ts) - 1];

}   // namespace ddge::util::meta
