module;

#include <functional>

export module ddge.utility.tuple.tuple_all_of;

import ddge.utility.tuple.tuple_select;
import ddge.utility.meta.concepts.tuple_like;

namespace ddge::util {

export template <
    meta::tuple_like_c Tuple_T,
    typename Projection_T = std::identity,
    typename Predicate_T>
auto tuple_all_of(Tuple_T&& tuple, Predicate_T predicate, Projection_T projection = {})
    -> bool
{
    return std::apply(
        [&]<typename... Ts>(Ts&&... values) {
            return (
                std::invoke(predicate, std::invoke(projection, std::forward<Ts>(values)))
                && ...
            );
        },
        std::forward<Tuple_T>(tuple)
    );
}

}   // namespace ddge::util
