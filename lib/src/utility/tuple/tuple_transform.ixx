module;

#include <functional>

export module utility.tuple.tuple_transform;

import utility.meta.concepts.tuple_like;

namespace util {

export template <meta::tuple_like_c Tuple_T, typename Transform_T>
constexpr auto tuple_transform(Tuple_T&& tuple, Transform_T transform)
{
    return std::apply(
        [&transform]<typename... Ts>(Ts&&... values) {
            return std::tuple(std::invoke(transform, std::forward<Ts>(values))...);
        },
        std::forward<Tuple_T>(tuple)
    );
}

}   // namespace util
