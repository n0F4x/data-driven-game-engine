module;

#include <functional>
#include <utility>

export module ddge.utility.tuple.tuple_for_each;

import ddge.utility.meta.concepts.tuple_like;

namespace ddge::util {

export template <meta::tuple_like_c Tuple_T, typename F>
constexpr auto tuple_for_each(Tuple_T&& tuple, F func) -> void
{
    auto&& [... elems]{ std::forward<Tuple_T>(tuple) };
    (std::invoke(func, std::forward_like<Tuple_T>(elems)), ...);
}

}   // namespace ddge::util
