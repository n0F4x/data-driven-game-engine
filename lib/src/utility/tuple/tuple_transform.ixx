module;

#include <functional>
#include <utility>

export module ddge.utility.tuple.tuple_transform;

import ddge.utility.meta.concepts.tuple_like;

namespace ddge::util {

export template <meta::tuple_like_c Tuple_T, typename Transform_T>
constexpr auto tuple_transform(Tuple_T&& tuple, Transform_T transform)
{
    auto&& [... elems]{ std::forward<Tuple_T>(tuple) };
    return std::tuple{ std::invoke(transform, std::forward_like<Tuple_T>(elems))... };
}

}   // namespace ddge::util
